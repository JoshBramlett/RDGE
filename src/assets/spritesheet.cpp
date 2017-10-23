#include <rdge/assets/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/sprite_group.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/graphics/animation.hpp>

#include <SDL_assert.h>
#include <nlohmann/json.hpp>

#include <utility>   // std::pair
#include <exception>
#include <sstream>

#define CHECK_REQ(j, field, type) do {                                          \
    if (j.count(#field) == 0) {                                                 \
        throw std::invalid_argument("missing required field \"" #field "\"");   \
    } else if (!j[#field].type()) {                                             \
        throw std::invalid_argument("\"" #field "\" failed " #type "() check"); \
    }                                                                           \
} while (false)

#define CHECK_OPT(j, field, type) do {                                          \
    if ((j.count(#field) > 0) && (!j[#field].type())) {                         \
        throw std::invalid_argument("\"" #field "\" failed " #type "() check"); \
    }                                                                           \
} while (false)


namespace rdge {

using namespace rdge::math;
using json = nlohmann::json;

struct region_data
{
    std::string  name;
    spritesheet_region value;
};

struct animation_data
{
    std::string name;
    Animation   value;
};

namespace {

struct parsed_region_data
{
    std::string name;
    uint32 x;
    uint32 y;
    uint32 width;
    uint32 height;
    math::vec2 origin;
};

struct parsed_frame
{
    std::string name;
    uint32 flip;
};

struct parsed_animation_data
{
    std::string name;
    Animation::PlayMode mode;
    uint32 interval;
    std::vector<parsed_frame> frames;
};

struct pyxel_edit_tile_data
{
    int32 index;
    uint32 x;
    uint32 y;
    uint32 flip;
    uint32 tile;
    uint32 rot;
};

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

// Parse and validate a single region
parsed_region_data
ParseRegion (const json& j)
{
    CHECK_REQ(j, name, is_string);
    CHECK_REQ(j, x, is_number_unsigned);
    CHECK_REQ(j, y, is_number_unsigned);
    CHECK_REQ(j, width, is_number_unsigned);
    CHECK_REQ(j, height, is_number_unsigned);
    CHECK_OPT(j, origin, is_array);

    parsed_region_data result;
    result.name = j["name"].get<std::string>();
    result.x = j["x"].get<uint32>();
    result.y = j["y"].get<uint32>();
    result.width = j["width"].get<uint32>();
    result.height = j["height"].get<uint32>();

    // NOTE: For optional params we must prepend type checking with counting
    //       the elements because the nlohmann/json library didn't feel the
    //       need to implement an exists() method.  Accessing an element by
    //       a key that does not exist has a side effect of constructing a
    //       null object in-place.
    if (j.count("origin") > 0)
    {
        const auto& origin = j["origin"];
        if (!origin[0].is_number_unsigned() || !origin[1].is_number_unsigned())
        {
            std::string msg("region \"" + result.name + "\" origin expects unsigned values");
            throw std::invalid_argument(msg);
        }

        auto tmp_x = origin[0].get<uint32>();
        auto tmp_y = origin[1].get<uint32>();
        if (tmp_x > result.width || tmp_y > result.height)
        {
            std::string msg("region \"" + result.name + "\" origin outside valid range");
            throw std::invalid_argument(msg);
        }

        result.origin.x = static_cast<float>(tmp_x);
        result.origin.y = static_cast<float>(tmp_y);
    }
    else
    {
        result.origin.x = static_cast<float>(result.width) * 0.5f;
        result.origin.y = static_cast<float>(result.height) * 0.5f;
    }

    return result;
}

parsed_animation_data
ParseAnimation (const json& j)
{
    CHECK_REQ(j, name, is_string);
    CHECK_REQ(j, interval, is_number_unsigned);
    CHECK_REQ(j, mode, is_string);
    CHECK_REQ(j, frames, is_array);

    parsed_animation_data result;
    result.name = j["name"].get<std::string>();
    result.interval = j["interval"].get<uint32>();

    if (!from_string(j["mode"].get<std::string>(), result.mode))
    {
        std::string msg("animation \"" + result.name + "\" mode invalid");
        throw std::invalid_argument(msg);
    }

    const auto& frames = j["frames"];
    for (const auto& frame : frames)
    {
        CHECK_REQ(frame, name, is_string);
        CHECK_OPT(frame, flip, is_number_unsigned);

        parsed_frame result_frame;
        result_frame.name = frame["name"].get<std::string>();
        result_frame.flip = 0;

        if (frame.count("flip") > 0)
        {
            result_frame.flip = frame["flip"].get<uint32>();
        }

        result.frames.push_back(result_frame);
    }

    return result;
}

pyxel_edit_tile_data
ParsePyxelEditTile (const json& j)
{
    CHECK_REQ(j, x, is_number_unsigned);
    CHECK_REQ(j, y, is_number_unsigned);
    CHECK_REQ(j, index, is_number);
    CHECK_REQ(j, tile, is_number_unsigned);
    CHECK_REQ(j, flipX, is_boolean);
    CHECK_REQ(j, rot, is_number);

    pyxel_edit_tile_data result;
    result.x = j["x"].get<uint32>();
    result.y = j["y"].get<uint32>();
    result.index = j["index"].get<uint32>();
    result.tile = j["tile"].get<uint32>();
    result.flip = (j["flipX"].get<bool>()) ? 1 : 0;
    result.rot = j["rot"].get<int32>();

    return result;
}

void
ProcessSpriteSheet (const json& j, SpriteSheet& sheet)
{
    sheet.region_count = (j.count("regions") > 0) ? j["regions"].size() : 0;
    if (sheet.region_count > 0)
    {
        RDGE_CALLOC(sheet.regions, sheet.region_count, nullptr);

        auto surface_size = sheet.surface.Size();
        const auto& j_regions = j["regions"];
        for (size_t i = 0; i < sheet.region_count; i++)
        {
            auto parsed = ParseRegion(j_regions[i]);

            // Validate values are within range
            if ((parsed.x + parsed.width > surface_size.w) ||
                (parsed.y + parsed.height > surface_size.h))
            {
                std::string msg("region \"" + parsed.name + "\" outside valid range");
                throw std::invalid_argument(msg);
            }

            auto& region = sheet.regions[i];
            region.name = parsed.name;
            region.value.clip = { static_cast<int32>(parsed.x),
                                  static_cast<int32>(parsed.y),
                                  static_cast<int32>(parsed.width),
                                  static_cast<int32>(parsed.height) };
            region.value.size = vec2(parsed.width, parsed.height);
            region.value.origin = parsed.origin;

            float x1 = normalize(parsed.x, surface_size.w);
            float x2 = normalize(parsed.x + parsed.width, surface_size.w);
            float y1 = normalize(parsed.y, surface_size.h);
            float y2 = normalize(parsed.y + parsed.height, surface_size.h);
            region.value.coords.bottom_left  = vec2(x1, y1);
            region.value.coords.bottom_right = vec2(x2, y1);
            region.value.coords.top_left     = vec2(x1, y2);
            region.value.coords.top_right    = vec2(x2, y2);
        }
    }

    sheet.animation_count = (j.count("animations") > 0) ? j["animations"].size() : 0;
    if (sheet.animation_count > 0)
    {
        RDGE_CALLOC(sheet.animations, sheet.animation_count, nullptr);

        const auto& j_animations = j["animations"];
        for (size_t i = 0; i < sheet.animation_count; i++)
        {
            auto parsed = ParseAnimation(j_animations[i]);

            auto& animation = sheet.animations[i];
            animation.name = parsed.name;
            animation.value.mode = parsed.mode;
            animation.value.interval = parsed.interval;

            for (const auto& parsed_frame : parsed.frames)
            {
                bool found = false;
                for (size_t ii = 0; ii < sheet.region_count; ii++)
                {
                    const auto& region = sheet.regions[ii];
                    if (region.name == parsed_frame.name)
                    {
                        auto region_copy = region.value;
                        region_copy.flip(static_cast<TexCoordsFlip>(parsed_frame.flip));
                        animation.value.frames.emplace_back(region_copy);

                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    std::ostringstream ss;
                    ss << "animation \"" << parsed.name << "\" cannot find "
                       << "frame \"" << parsed_frame.name << "\" in region list";
                    throw std::invalid_argument(ss.str());
                }
            }
        }
    }
}

void
ProcessTilemap (const json& j, SpriteSheet& sheet)
{
    CHECK_REQ(j, tileswide, is_number_unsigned);
    CHECK_REQ(j, tileshigh, is_number_unsigned);
    CHECK_REQ(j, tilewidth, is_number_unsigned);
    CHECK_REQ(j, tileheight, is_number_unsigned);
    CHECK_REQ(j, layers, is_array);

    auto tileswide = j["tileswide"].get<uint32>();
    auto tileshigh = j["tileshigh"].get<uint32>();
    auto tilewidth = j["tilewidth"].get<uint32>();
    auto tileheight = j["tileheight"].get<uint32>();

    auto surface_size = sheet.surface.Size();
    if ((surface_size.w % tilewidth > 0) || (surface_size.h % tileheight > 0))
    {
        std::string msg("tilemap expects no surface padding");
        throw std::invalid_argument(msg);
    }

    uint32 region_rows = surface_size.h / tileheight;
    uint32 region_cols = surface_size.w / tilewidth;
    sheet.region_count = region_rows * region_cols;

    if (sheet.region_count == 0)
    {
        std::string msg("tilemap tileheight or tilewidth are invalid");
        throw std::invalid_argument(msg);
    }

    RDGE_CALLOC(sheet.regions, sheet.region_count, nullptr);
    for (size_t row = 0; row < region_rows; row++)
    {
        for (size_t col = 0; col < region_cols; col++)
        {
            auto& region = sheet.regions[(row * region_cols) + col];

            std::ostringstream ss;
            ss << "tile_" << row << "_" << col;
            region.name = ss.str();

            auto x = col * tilewidth;
            auto y = row * tileheight;
            region.value.clip = { static_cast<int32>(x),
                                  static_cast<int32>(y),
                                  static_cast<int32>(tilewidth),
                                  static_cast<int32>(tileheight) };
            region.value.size = vec2(tilewidth, tileheight);
            region.value.origin.x = region.value.size.w * 0.5f;
            region.value.origin.y = region.value.size.h * 0.5f;

            float x1 = normalize(x, surface_size.w);
            float x2 = normalize(x + tilewidth, surface_size.w);
            float y1 = normalize(y, surface_size.h);
            float y2 = normalize(y + tileheight, surface_size.h);
            region.value.coords.bottom_left  = vec2(x1, y1);
            region.value.coords.bottom_right = vec2(x2, y1);
            region.value.coords.top_left     = vec2(x1, y2);
            region.value.coords.top_right    = vec2(x2, y2);
        }
    }

    const auto& j_layers = j["layers"];
    sheet.tilemap.layer_count = j_layers.size();
    sheet.tilemap.tile_pitch = tileswide;
    sheet.tilemap.tile_count = tileswide * tileshigh;
    sheet.tilemap.tile_size = vec2(tilewidth, tileheight);

    if (sheet.tilemap.layer_count > tilemap_data::MAX_LAYER_COUNT)
    {
        std::ostringstream ss;
        ss << "tilemap has more layers than are currently supported"
           << " count=" << sheet.tilemap.layer_count
           << " max=" << tilemap_data::MAX_LAYER_COUNT;
        throw std::invalid_argument(ss.str());
    }

    for (size_t i = 0; i < sheet.tilemap.layer_count; i++)
    {
        const auto& j_layer = j_layers[0];

        CHECK_REQ(j_layer, number, is_number_unsigned);
        CHECK_REQ(j_layer, tiles, is_array);

        auto layer_index = j_layer["number"].get<uint32>();
        if (layer_index >= sheet.tilemap.layer_count)
        {
            std::string msg("tilemap layer index out of range");
            throw std::invalid_argument(msg);
        }

        const auto& j_tiles = j_layer["tiles"];
        if (j_tiles.size() != sheet.tilemap.tile_count)
        {
            std::string msg("tilemap tile entry count does not match definition");
            throw std::invalid_argument(msg);
        }

        auto& layer = sheet.tilemap.layers[layer_index];
        RDGE_CALLOC(layer.tiles, sheet.tilemap.tile_count, nullptr);
        for (const auto& j_tile : j_tiles)
        {
            auto parsed = ParsePyxelEditTile(j_tile);
            if (parsed.tile >= sheet.region_count)
            {
                std::string msg("tile region out of bounds. value=" +
                                std::to_string(parsed.tile));
                throw std::invalid_argument(msg);
            }

            auto& tile = layer.tiles[parsed.index];
            tile.index = parsed.index;
            tile.location = { parsed.x, parsed.y };

            if (tile.index < 0)
            {
                tile.index = tilemap_data::INVALID_TILE;
                tile.coords.top_left     = { 0.f, 0.f };
                tile.coords.bottom_left  = { 0.f, 0.f };
                tile.coords.bottom_right = { 0.f, 0.f };
                tile.coords.top_right    = { 0.f, 0.f };
            }
            else
            {
                tile.coords = sheet.regions[parsed.tile].value.coords;

                tile.coords.flip(static_cast<TexCoordsFlip>(parsed.flip));
                tile.coords.rotate(static_cast<TexCoordsRotation>(parsed.rot));
            }
        }
    }
}

} // anonymous namespace

tilemap_data::~tilemap_data (void) noexcept
{
    for (size_t i = 0; i < this->layer_count; i++)
    {
        RDGE_FREE(this->layers[i].tiles, nullptr);
    }
}

tilemap_data::tilemap_data (const tilemap_data& other)
    : layer_count(other.layer_count)
    , tile_count(other.tile_count)
    , tile_pitch(other.tile_pitch)
    , tile_size(other.tile_size)
{
    for (size_t i = 0; i < other.layer_count; i++)
    {
        const auto& src = other.layers[i];
        auto& dest = this->layers[i];

        RDGE_MALLOC_N(dest.tiles, other.tile_count, nullptr);
        memcpy(dest.tiles, src.tiles, sizeof(tile) * other.tile_count);
    }
}

tilemap_data::tilemap_data (tilemap_data&& other) noexcept
    : layer_count(other.layer_count)
    , tile_count(other.tile_count)
    , tile_pitch(other.tile_pitch)
    , tile_size(other.tile_size)
{
    for (size_t i = 0; i < this->layer_count; i++)
    {
        this->layers[i].tiles = other.layers[i].tiles;
        other.layers[i].tiles = nullptr;
    }

    other.layer_count = 0;
}

tilemap_data&
tilemap_data::operator= (const tilemap_data& rhs)
{
    if (this != &rhs)
    {
        for (size_t i = 0; i < MAX_LAYER_COUNT; i++)
        {
            const auto& src = rhs.layers[i];
            auto& dest = this->layers[i];

            if (dest.tiles)
            {
                SDL_assert(i < this->layer_count);

                if (src.tiles)
                {
                    SDL_assert(i < rhs.layer_count);

                    RDGE_REALLOC(dest.tiles, rhs.tile_count, nullptr);
                    memcpy(dest.tiles, src.tiles, sizeof(tile) * rhs.tile_count);
                }
                else
                {
                    RDGE_FREE(dest.tiles, nullptr);
                    dest.tiles = nullptr;
                }
            }
            else
            {
                SDL_assert(i >= this->layer_count);

                if (src.tiles)
                {
                    SDL_assert(i < rhs.layer_count);

                    RDGE_MALLOC_N(dest.tiles, rhs.tile_count, nullptr);
                    memcpy(dest.tiles, src.tiles, sizeof(tile) * rhs.tile_count);
                }
            }
        }

        this->layer_count = rhs.layer_count;
        this->tile_count = rhs.tile_count;
        this->tile_pitch = rhs.tile_pitch;
        this->tile_size = rhs.tile_size;
    }

    return *this;
}

tilemap_data&
tilemap_data::operator= (tilemap_data&& rhs) noexcept
{
    if (this != &rhs)
    {
        for (size_t i = 0; i < MAX_LAYER_COUNT; i++)
        {
            std::swap(this->layers[i].tiles, rhs.layers[i].tiles);
        }

        std::swap(this->layer_count, rhs.layer_count);
        std::swap(this->tile_count, rhs.tile_count);
        std::swap(this->tile_pitch, rhs.tile_pitch);
        std::swap(this->tile_size, rhs.tile_size);
    }

    return *this;
}

SpriteSheet::SpriteSheet (const std::vector<uint8>& msgpack, Surface surface)
    : surface(surface)
    , texture(std::make_shared<Texture>(this->surface))
{
    try
    {
        json j = json::from_msgpack(msgpack);
        CHECK_REQ(j, type, is_string);

        auto sheet_type = j["type"].get<std::string>();
        if (sheet_type == "spritesheet")
        {
            ProcessSpriteSheet(j, *this);
        }
        else if (sheet_type == "tilemap")
        {
            ProcessTilemap(j, *this);
        }
        else
        {
            throw std::invalid_argument("invalid sheet type");
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_FREE(this->regions, nullptr);
        RDGE_FREE(this->animations, nullptr);

        RDGE_THROW(ex.what());
    }
}

SpriteSheet::SpriteSheet (const char* filepath)
{
    try
    {
        auto rwops = rwops_base::from_file(filepath, "rt");
        auto sz = rwops.size();

        std::string file_data(sz + 1, '\0');
        rwops.read(file_data.data(), sizeof(char), sz);

        const auto j = json::parse(file_data);
        CHECK_REQ(j, image_path, is_string);
        CHECK_REQ(j, type, is_string);

        auto image_path = j["image_path"].get<std::string>();
        this->surface = Surface(image_path);
        this->texture = std::make_shared<Texture>(this->surface);

        auto sheet_type = j["type"].get<std::string>();
        if (sheet_type == "spritesheet")
        {
            ProcessSpriteSheet(j, *this);
        }
        else if (sheet_type == "tilemap")
        {
            ProcessTilemap(j, *this);
        }
        else
        {
            throw std::invalid_argument("invalid sheet type");
        }
    }
    catch (const std::logic_error& ex)
    {
        // Catches domain_error, out_of_range, invalid_argument.
        // No need to handle them differently at this time.

        RDGE_FREE(this->regions, nullptr);
        RDGE_FREE(this->animations, nullptr);

        RDGE_THROW(ex.what());
    }
}

SpriteSheet::~SpriteSheet (void) noexcept
{
    RDGE_FREE(this->regions, nullptr);
    RDGE_FREE(this->animations, nullptr);
}

SpriteSheet::SpriteSheet (SpriteSheet&& other) noexcept
    : surface(std::move(other.surface))
    , texture(std::move(other.texture))
    , regions(other.regions)
    , animations(other.animations)
    , tilemap(std::move(other.tilemap))
{
    other.regions = nullptr;
    other.animations = nullptr;

    other.region_count = 0;
    other.animation_count = 0;
}

SpriteSheet&
SpriteSheet::operator= (SpriteSheet&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(this->surface, rhs.surface);
        std::swap(this->texture, rhs.texture);
        std::swap(this->regions, rhs.regions);
        std::swap(this->animations, rhs.animations);
        this->tilemap = std::move(rhs.tilemap);

        rhs.region_count = 0;
        rhs.animation_count = 0;
    }

    return *this;
}

const spritesheet_region&
SpriteSheet::operator[] (const std::string& name) const
{
    for (size_t i = 0; i < this->region_count; i++)
    {
        const auto& region = this->regions[i];
        if (region.name == name)
        {
            return region.value;
        }
    }

    RDGE_THROW("SpriteSheet region lookup failed. key=" + name);
}

const Animation&
SpriteSheet::GetAnimation (const std::string& name) const
{
    for (size_t i = 0; i < this->animation_count; i++)
    {
        const auto& animation = this->animations[i];
        if (animation.name == name)
        {
            return animation.value;
        }
    }

    RDGE_THROW("SpriteSheet animation lookup failed. key=" + name);
}

const Animation&
SpriteSheet::GetAnimation (int32 animation_id) const
{
    SDL_assert(animation_id >= 0);
    SDL_assert(animation_id < (int32)this->animation_count);

    return this->animations[animation_id].value;
}

std::unique_ptr<Sprite>
SpriteSheet::CreateSprite (const std::string& name, const math::vec3& pos) const
{
    SDL_assert(this->texture != nullptr);

    const auto& part = (*this)[name]; // Can throw if lookup fails
    return std::make_unique<Sprite>(pos, part.size, this->texture, part.coords);
}

std::unique_ptr<SpriteGroup>
SpriteSheet::CreateSpriteChain (const std::string& name,
                                const math::vec3&  pos,
                                const math::vec2&  to_fill) const
{
    SDL_assert(this->texture != nullptr);

    const auto& part = (*this)[name]; // Can throw if lookup fails
    auto size = part.size;
    auto group = std::make_unique<SpriteGroup>();

    uint32 rows = (to_fill.h > size.h) ? static_cast<uint32>(to_fill.h / size.h) + 1 : 1;
    uint32 cols = (to_fill.w > size.w) ? static_cast<uint32>(to_fill.w / size.w) + 1 : 1;

    float x = pos.x;
    float y = pos.y;
    for (uint32 ri = 0; ri < rows; ++ri)
    {
        for (uint32 ci = 0; ci < cols; ++ci)
        {
            group->AddSprite(std::make_shared<Sprite>(math::vec3(x, y, pos.z),
                                                      size,
                                                      this->texture,
                                                      part.coords));

            x += size.w;
        }

        x = pos.x;
        y += size.y;
    }

    return group;
}

} // namespace rdge
