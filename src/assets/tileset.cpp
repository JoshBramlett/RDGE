#include <rdge/assets/tileset.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/json.hpp>

#include <vector>
#include <sstream>

namespace rdge {

using namespace rdge::math;
using json = nlohmann::json;

namespace {

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

void
ProcessTileset (const json& j, Tileset& tileset)
{
    JSON_VALIDATE_REQUIRED(j, tileheight, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tilewidth, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, spacing, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, margin, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tilecount, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, columns, is_number_unsigned);
    JSON_VALIDATE_OPTIONAL(j, tiles, is_array);

    auto tile_width = j["tilewidth"].get<size_t>();
    auto tile_height = j["tileheight"].get<size_t>();

    tileset.spacing = j["spacing"].get<decltype(tileset.spacing)>();
    tileset.margin = j["margin"].get<decltype(tileset.margin)>();
    tileset.tile_count = j["tilecount"].get<decltype(tileset.tile_count)>();
    tileset.cols = j["columns"].get<decltype(tileset.cols)>();
    tileset.rows = tileset.tile_count / tileset.cols;
    tileset.tile_size = math::vec2(static_cast<float>(tile_width),
                                   static_cast<float>(tile_height));

    if (tileset.spacing != 0)
    {
        throw std::invalid_argument("Tile spacing not currently supported");
    }

    if (RDGE_UNLIKELY(!RDGE_TCALLOC(tileset.tiles,
                                    tileset.tile_count,
                                    memory_bucket_assets)))
    {
        RDGE_THROW_ALLOC_FAILED();
    }

    // build tile definitions first - they're required for the animations
    auto surface_size = tileset.surface->Size();
    for (size_t row = 0; row < tileset.rows; row++)
    {
        for (size_t col = 0; col < tileset.cols; col++)
        {
            auto x = tileset.margin + (col * tile_width);
            auto y = tileset.margin + (row * tile_height);

            float x1 = normalize(x, surface_size.w);
            float x2 = normalize(x + tile_width, surface_size.w);
            float y1 = normalize(y, surface_size.h);
            float y2 = normalize(y + tile_height, surface_size.h);

            auto& tile = tileset.tiles[(row * tileset.cols) + col];
            tile.animation_index = -1;
            tile.uv.bottom_left  = vec2(x1, y1);
            tile.uv.bottom_right = vec2(x2, y1);
            tile.uv.top_left     = vec2(x1, y2);
            tile.uv.top_right    = vec2(x2, y2);
        }
    }

    if (j.count("tiles"))
    {
        const auto& j_tiles = j["tiles"];
        tileset.animation_count = j_tiles.size();

        // get the total number of frames for all animations
        for (size_t i = 0; i < tileset.animation_count; i++)
        {
            const auto& j_tile = j_tiles[i];
            if (j_tile.count("animation"))
            {
                const auto& j_animation = j_tile["animation"];
                tileset.frame_count += j_animation.size();
            }
        }

        if (tileset.frame_count > 0)
        {
            if (RDGE_UNLIKELY(!RDGE_TCALLOC(tileset.animations,
                                            tileset.animation_count,
                                            memory_bucket_assets)))
            {
                RDGE_THROW_ALLOC_FAILED();
            }

            if (RDGE_UNLIKELY(!RDGE_TCALLOC(tileset.frames,
                                            tileset.frame_count,
                                            memory_bucket_assets)))
            {
                RDGE_THROW_ALLOC_FAILED();
            }

            size_t total_frame_count = 0;
            for (size_t anim_idx = 0; anim_idx < tileset.animation_count; anim_idx++)
            {
                const auto& j_tile = j_tiles[anim_idx];
                JSON_VALIDATE_REQUIRED(j_tile, id, is_number);
                JSON_VALIDATE_OPTIONAL(j_tile, animation, is_array);

                auto parent_id = j_tile["id"].get<uint32>();
                tileset.tiles[parent_id].animation_index = static_cast<int32>(anim_idx);

                auto& animation = tileset.animations[anim_idx];
                if (j_tile.count("animation"))
                {
                    const auto& j_animation = j_tile["animation"];
                    animation.frame_count = j_animation.size();
                    animation.frames = tileset.frames + total_frame_count;

                    for (size_t frame_idx = 0; frame_idx < animation.frame_count; frame_idx++)
                    {
                        const auto& j_frame = j_animation[frame_idx];
                        JSON_VALIDATE_REQUIRED(j_frame, tileid, is_number);
                        JSON_VALIDATE_REQUIRED(j_frame, duration, is_number);

                        auto& frame = animation.frames[frame_idx];
                        frame.tile_id = j_frame["tileid"].get<decltype(frame.tile_id)>();
                        frame.duration = j_frame["duration"].get<decltype(frame.duration)>();
                    }
                }
                else
                {
                    animation.frame_count = 0;
                    animation.frames = nullptr;
                }

                total_frame_count += animation.frame_count;
            }
        }
    }
}

} // anonymous namespace

Tileset::Tileset (const char* filepath)
{
    try
    {
        const auto j = json::parse(GetTextFileContent(filepath));
        JSON_VALIDATE_REQUIRED(j, image, is_string);
        JSON_VALIDATE_REQUIRED(j, imageheight, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j, imagewidth, is_number_unsigned);

        void* pnew = RDGE_MALLOC(sizeof(Surface), memory_bucket_assets);
        if (RDGE_UNLIKELY(!pnew))
        {
            RDGE_THROW_ALLOC_FAILED();
        }

        Surface* raw = new (pnew) Surface(j["image"].get<std::string>());
        this->surface = shared_asset<Surface>(raw);

        // sanity check
        if (this->surface->Width() != j["imagewidth"].get<size_t>() ||
            this->surface->Height() != j["imageheight"].get<size_t>())
        {
            throw std::invalid_argument("Surface size mismatch");
        }

        ProcessTileset(j, *this);
    }
    catch (const std::logic_error& ex)
    {
        RDGE_THROW(ex.what());
    }
}

Tileset::Tileset (const std::vector<uint8>& msgpack, PackFile& packfile)
{
    try
    {
        json j = json::from_msgpack(msgpack);
        JSON_VALIDATE_REQUIRED(j, image_table_id, is_number);
        JSON_VALIDATE_REQUIRED(j, imageheight, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j, imagewidth, is_number_unsigned);

        this->surface = packfile.GetAsset<Surface>(j["image_table_id"].get<int32>());

        // sanity check
        if (this->surface->Width() != j["imagewidth"].get<size_t>() ||
            this->surface->Height() != j["imageheight"].get<size_t>())
        {
            throw std::invalid_argument("Surface size mismatch");
        }

        ProcessTileset(j, *this);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

Tileset::~Tileset (void) noexcept
{
    RDGE_FREE(this->tiles, memory_bucket_assets);
    RDGE_FREE(this->animations, memory_bucket_assets);
    RDGE_FREE(this->frames, memory_bucket_assets);
}

Tileset::Tileset (Tileset&& other) noexcept
    : tile_size(other.tile_size)
    , rows(other.rows)
    , cols(other.cols)
    , spacing(other.spacing)
    , margin(other.margin)
    , tiles(other.tiles)
    , tile_count(other.tile_count)
    , animations(other.animations)
    , animation_count(other.animation_count)
    , frames(other.frames)
    , frame_count(other.frame_count)
    , surface(std::move(other.surface))
{
    other.tiles = nullptr;
    other.animations = nullptr;
    other.frames = nullptr;
}

Tileset&
Tileset::operator= (Tileset&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->tile_size = rhs.tile_size;
        this->rows = rhs.rows;
        this->cols = rhs.cols;
        this->spacing = rhs.spacing;
        this->margin = rhs.margin;
        this->tile_count = rhs.tile_count;
        this->animation_count = rhs.animation_count;
        this->frame_count = rhs.frame_count;
        this->surface = std::move(rhs.surface);

        std::swap(this->tiles, rhs.tiles);
        std::swap(this->animations, rhs.animations);
        std::swap(this->frames, rhs.frames);
    }

    return *this;
}

} // namespace rdge
