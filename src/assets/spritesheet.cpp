#include <rdge/assets/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/sprite_group.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>
#include <nlohmann/json.hpp>

#include <utility>   // std::pair
#include <exception>
#include <sstream>

namespace rdge {

using namespace rdge::math;
using json = nlohmann::json;

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

enum flip_state
{
    flip_none = 0,
    flip_horizontal,
    flip_vertical
};

struct parsed_frame
{
    std::string name;
    flip_state flip;
};

struct parsed_animation_data
{
    std::string name;
    Animation::PlayMode mode;
    uint32 interval;
    std::vector<parsed_frame> frames;
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
    if (j.count("name") == 0 || !j["name"].is_string())
    {
        std::string msg("region name is missing or is an invalid type");
        throw std::invalid_argument(msg);
    }

    parsed_region_data result;
    result.name = j["name"].get<std::string>();

    // Validate for unsigned to prevent overflow issues
    if (!j["x"].is_number_unsigned() || !j["y"].is_number_unsigned() ||
        !j["width"].is_number_unsigned() || !j["height"].is_number_unsigned())
    {
        std::string msg("region \"" + result.name + "\" dimensions expect unsigned values");
        throw std::invalid_argument(msg);
    }

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
        if (!j["origin"].is_array())
        {
            std::string msg("region \"" + result.name + "\" origin expects an array");
            throw std::invalid_argument(msg);
        }

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
    if (j.count("name") == 0 || !j["name"].is_string())
    {
        std::string msg("animation name is missing or is an invalid type");
        throw std::invalid_argument(msg);
    }

    parsed_animation_data result;
    result.name = j["name"].get<std::string>();

    if (!j["interval"].is_number_unsigned())
    {
        std::string msg("animation \"" + result.name + "\" interval expects unsigned values");
        throw std::invalid_argument(msg);
    }

    result.interval = j["interval"].get<uint32>();

    if (!from_string(j["mode"].get<std::string>(), result.mode))
    {
        std::string msg("animation \"" + result.name + "\" mode invalid");
        throw std::invalid_argument(msg);
    }

    const auto& frames = j["frames"];
    for (const auto& frame : frames)
    {
        parsed_frame result_frame;
        result_frame.name = frame["name"].get<std::string>();
        result_frame.flip = flip_none;

        if (frame.count("flip") > 0)
        {
            auto flip_str = to_lower(frame["flip"].get<std::string>());
            if (flip_str == "horizontal")
            {
                result_frame.flip = flip_horizontal;
            }
            else if (flip_str == "vertical")
            {
                result_frame.flip = flip_vertical;
            }
            else
            {
                std::ostringstream ss;
                ss << "animation \"" << result.name << "\" at "
                   << "frame \"" << result_frame.name << "\" has invalid flip value";
                throw std::invalid_argument(ss.str());
            }
        }

        result.frames.push_back(result_frame);
    }

    return result;
}

void
ProcessJson (const json& j, SpriteSheet& sheet)
{
    sheet.region_count = (j.count("texture_parts") > 0) ? j["texture_parts"].size() : 0;
    if (sheet.region_count > 0)
    {
        RDGE_CALLOC(sheet.regions, sheet.region_count, nullptr);

        auto surface_size = sheet.surface.Size();
        const auto& json_regions = j["texture_parts"];
        for (size_t i = 0; i < sheet.region_count; i++)
        {
            auto parsed = ParseRegion(json_regions[i]);

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

        const auto& json_animations = j["animations"];
        for (size_t i = 0; i < sheet.animation_count; i++)
        {
            auto parsed = ParseAnimation(json_animations[i]);

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
                        if (parsed_frame.flip == flip_horizontal)
                        {
                            animation.value.frames.emplace_back(region.value.flip_horizontal());
                        }
                        else if (parsed_frame.flip == flip_vertical)
                        {
                            animation.value.frames.emplace_back(region.value.flip_vertical());
                        }
                        else
                        {
                            animation.value.frames.emplace_back(region.value);
                        }

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

} // anonymous namespace

SpriteSheet::SpriteSheet (const std::vector<uint8>& msgpack, Surface surface)
    : surface(surface)
    , texture(std::make_shared<Texture>(this->surface))
{
    try
    {
        json j = json::from_msgpack(msgpack);
        ProcessJson(j, *this);
    }
    catch (const std::exception& ex)
    {
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

        auto image_path = j["image_path"].get<std::string>();
        this->surface = Surface(image_path);
        this->texture = std::make_shared<Texture>(this->surface);

        ProcessJson(j, *this);
    }
    catch (const std::logic_error& ex)
    {
        // Catches domain_error, out_of_range, invalid_argument.
        // No need to handle them differently at this time.

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

        rhs.region_count = 0;
        rhs.animation_count = 0;
    }

    return *this;
}

const texture_part&
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
