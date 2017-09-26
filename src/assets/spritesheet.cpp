#include <rdge/assets/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/sprite_group.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>
#include <nlohmann/json.hpp>

#include <utility>   // std::pair
#include <exception>
#include <sstream>

using json = nlohmann::json;

namespace {

using namespace rdge;

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

// Process and validate a single region
void
ProcessRegion (texture_part& region, const json& j, const math::uivec2& surface_size, uint32 scale)
{
    auto name = j["name"].get<std::string>();

    // Validate for unsigned to prevent overflow issues
    if (!j["x"].is_number_unsigned() || !j["y"].is_number_unsigned() ||
        !j["width"].is_number_unsigned() || !j["height"].is_number_unsigned())
    {
        throw std::invalid_argument("region \"" + name + "\" expects unsigned");
    }

    auto x = j["x"].get<uint32>();
    auto y = j["y"].get<uint32>();
    auto w = j["width"].get<uint32>();
    auto h = j["height"].get<uint32>();

    // Validate values are within range
    if ((x + w > surface_size.w) || (y + h > surface_size.h))
    {
        throw std::invalid_argument("region \"" + name + "\" has invalid dimensions");
    }

    // Optional origin
    math::vec2 origin;
    if (j.count("origin") > 0 && j["origin"].is_array())
    {
        const auto& values = j["origin"];
        if (!values[0].is_number_unsigned() || !values[1].is_number_unsigned())
        {
            throw std::invalid_argument("region \"" + name + "\" origin expects unsigned");
        }

        origin.x = values[0].get<uint32>();
        origin.y = values[1].get<uint32>();

        if (origin.x > w || origin.y > h)
        {
            throw std::invalid_argument("region \"" + name + "\" invalid origin");
        }
    }
    else
    {
        origin.x = static_cast<float>(w) * 0.5f;
        origin.y = static_cast<float>(h) * 0.5f;
    }

    region.name = name;
    region.clip = { static_cast<int32>(x), static_cast<int32>(y),
                    static_cast<int32>(w), static_cast<int32>(h) };
    region.size = { w * scale, h * scale };
    region.origin = origin * scale;
    region.coords.bottom_left  = math::vec2(::normalize(x, surface_size.w),
                                            ::normalize(y, surface_size.h));
    region.coords.bottom_right = math::vec2(::normalize(x + w, surface_size.w),
                                            ::normalize(y, surface_size.h));
    region.coords.top_left     = math::vec2(::normalize(x, surface_size.w),
                                            ::normalize(y + h, surface_size.h));
    region.coords.top_right    = math::vec2(::normalize(x + w, surface_size.w),
                                            ::normalize(y + h, surface_size.h));
}

void
ProcessAnimation (Animation& animation, const json& j, const texture_part* regions, size_t region_count)
{
    auto name = j["name"].get<std::string>();

    // Validate for unsigned to prevent overflow issues
    if (!j["interval"].is_number_unsigned())
    {
        throw std::invalid_argument("animation \"" + name + "\" expects unsigned");
    }

    if (!from_string(j["mode"].get<std::string>(), animation.mode))
    {
        throw std::invalid_argument("animation \"" + name + "\" mode invalid");
    }

    animation.interval = j["interval"].get<uint32>();

    const auto& frames = j["frames"];
    for (const auto& frame : frames)
    {
        auto frame_name = frame["name"].get<std::string>();

        bool found = false;
        for (size_t i = 0; i < region_count; i++)
        {
            const auto& region = regions[i];
            if (region.name == frame_name)
            {
                if (frame.count("flip") > 0 && frame["flip"].is_string())
                {
                    auto flip = to_lower(frame["flip"].get<std::string>());
                    if (flip == "horizontal")
                    {
                        animation.frames.emplace_back(region.flip_horizontal());
                    }
                    else if (flip == "vertical")
                    {
                        animation.frames.emplace_back(region.flip_vertical());
                    }
                    else
                    {
                        std::ostringstream ss;
                        ss << "animation \"" << name << "\" at "
                           << "frame \"" << frame_name << "\" has invalid flip value";
                        throw std::invalid_argument(ss.str());
                    }
                }
                else
                {
                    animation.frames.emplace_back(region);
                }

                found = true;
                break;
            }
        }

        if (!found)
        {
            std::ostringstream ss;
            ss << "animation \"" << name << "\" cannot find "
               << "frame \"" << frame_name << "\" in texture_part list";
            throw std::invalid_argument(ss.str());
        }
    }
}

// Process and validate a single texture_part
std::pair<std::string, texture_part>
ProcessTexturePart (const json& part, const math::uivec2& surface_size, uint32 scale)
{
    auto name = part["name"].get<std::string>();

    // Validate for unsigned to prevent overflow issues
    if (!part["x"].is_number_unsigned() || !part["y"].is_number_unsigned() ||
        !part["width"].is_number_unsigned() || !part["height"].is_number_unsigned())
    {
        throw std::invalid_argument("texture_part \"" + name + "\" expects unsigned");
    }

    auto x = part["x"].get<uint32>();
    auto y = part["y"].get<uint32>();
    auto w = part["width"].get<uint32>();
    auto h = part["height"].get<uint32>();

    // Validate values are within range
    if ((x + w > surface_size.w) || (y + h > surface_size.h))
    {
        throw std::invalid_argument("texture_part \"" + name + "\" has invalid dimensions");
    }

    // Optional origin
    math::vec2 origin;
    if (part.count("origin") > 0 && part["origin"].is_array())
    {
        const auto& values = part["origin"];
        SDL_assert(values[0].is_number_unsigned());
        SDL_assert(values[1].is_number_unsigned());

        origin.x = values[0].get<uint32>();
        origin.y = values[1].get<uint32>();

        SDL_assert(origin.x <= w && origin.y <= h);
    }
    else
    {
        origin.x = static_cast<float>(w) * 0.5f;
        origin.y = static_cast<float>(h) * 0.5f;
    }

    texture_part result;
    result.name = name;
    result.clip = { static_cast<int32>(x), static_cast<int32>(y),
                    static_cast<int32>(w), static_cast<int32>(h) };
    result.size = { w * scale, h * scale };
    result.origin = origin * scale;
    result.coords.bottom_left  = math::vec2(::normalize(x, surface_size.w),
                                            ::normalize(y, surface_size.h));
    result.coords.bottom_right = math::vec2(::normalize(x + w, surface_size.w),
                                            ::normalize(y, surface_size.h));
    result.coords.top_left     = math::vec2(::normalize(x, surface_size.w),
                                            ::normalize(y + h, surface_size.h));
    result.coords.top_right    = math::vec2(::normalize(x + w, surface_size.w),
                                            ::normalize(y + h, surface_size.h));

    return std::make_pair(name, result);
}

} // anonymous namespace

namespace rdge {

SpriteSheet::SpriteSheet (const std::vector<uint8>& msgpack, Surface surface)
    : surface(surface)
    , texture(std::make_shared<Texture>(this->surface))
{
    try
    {
        json j = json::from_msgpack(msgpack);

        this->region_count = j["texture_parts"].size();
        if (this->region_count > 0)
        {
            RDGE_CALLOC(this->regions, this->region_count, nullptr);

            auto surface_size = this->surface.Size();
            const auto& json_regions = j["texture_parts"];
            for (size_t i = 0; i < this->region_count; i++)
            {
                ProcessRegion(this->regions[i], json_regions[i], surface_size, 1);
            }
        }

        this->animation_count = j["animations"].size();
        if (this->animation_count > 0)
        {
            RDGE_CALLOC(this->animations, this->animation_count, nullptr);

            const auto& json_animations = j["animations"];
            for (size_t i = 0; i < this->animation_count; i++)
            {
                ProcessAnimation(this->animations[i],
                                 json_animations[i],
                                 this->regions,
                                 this->region_count);
            }
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

SpriteSheet::SpriteSheet (const std::string& filepath, bool scale_for_hidpi)
{
    try
    {
        auto config = rdge::util::read_text_file(filepath.c_str());
        if (config.empty())
        {
            throw std::invalid_argument("File does not exist.");
        }

        const auto j = json::parse(config);
        this->image_path = j["image_path"].get<std::string>();

        // NOTE: For optional params we must prepend type checking with counting
        //       the elements because the nlohmann/json library didn't feel the
        //       need to implement an exists() method.  Accessing an element by
        //       a key that does not exist has a side effect of constructing a
        //       null object in-place.
        uint32 scale = scale_for_hidpi ? 2 : 1;
        if (j.count("image_scale") > 0 && j["image_scale"].is_number_unsigned())
        {
            this->image_scale = j["image_scale"].get<uint32>();
            scale *= this->image_scale;

            SDL_assert(math::is_pot(this->image_scale));
        }

        this->surface = Surface(this->image_path);
        this->texture = std::make_shared<Texture>(this->surface);
        auto surface_size = this->surface.Size();

        const auto& texture_parts = j["texture_parts"];
        for (const auto& part : texture_parts)
        {
            auto inserted = m_parts.emplace(ProcessTexturePart(part, surface_size, scale));
            if (inserted.second == false)
            {
                std::ostringstream ss;
                ss << "Part could not be added to the collection. "
                   << "check to see if there are duplicate keys. "
                   << "key=" << inserted.first->first;

                throw std::invalid_argument(ss.str());
            }
        }

        // TODO This could be moved to it's own method
        // TODO Add test cases
        if (j.count("animations") > 0)
        {
            const auto& animations = j["animations"];
            for (const auto& animation : animations)
            {
                auto name = animation["name"].get<std::string>();

                // Validate for unsigned to prevent overflow issues
                if (!animation["interval"].is_number_unsigned())
                {
                    throw std::invalid_argument("animation \"" + name + "\" expects unsigned");
                }

                Animation::PlayMode mode;
                if (!from_string(animation["mode"].get<std::string>(), mode))
                {
                    throw std::invalid_argument("animation \"" + name + "\" mode invalid");
                }

                Animation value(mode, animation["interval"].get<uint32>());

                const auto& frames = animation["frames"];
                for (const auto& frame : frames)
                {
                    auto frame_name = frame["name"].get<std::string>();
                    auto search = m_parts.find(frame_name);
                    if (search == m_parts.end())
                    {
                        std::ostringstream ss;
                        ss << "animation \"" << name << "\" cannot find "
                           << "frame \"" << frame_name << "\" in texture_part list";
                        throw std::invalid_argument(ss.str());
                    }

                    const auto& frame_part = search->second;
                    if (frame.count("flip") > 0 && frame["flip"].is_string())
                    {
                        auto flip = to_lower(frame["flip"].get<std::string>());
                        if (flip == "horizontal")
                        {
                            value.frames.emplace_back(frame_part.flip_horizontal());
                        }
                        else if (flip == "vertical")
                        {
                            value.frames.emplace_back(frame_part.flip_vertical());
                        }
                        else
                        {
                            std::ostringstream ss;
                            ss << "animation \"" << name << "\" at "
                               << "frame \"" << frame_name << "\" has invalid flip value";
                            throw std::invalid_argument(ss.str());
                        }
                    }
                    else
                    {
                        value.frames.emplace_back(frame_part);
                    }
                }

                m_animations.emplace(name, value);
            }
        }
    }
    catch (const std::logic_error& ex)
    {
        // Catches domain_error, out_of_range, invalid_argument.  No need to handle
        // them differently at this time.

        RDGE_THROW(ex.what());
    }
}

SpriteSheet::~SpriteSheet (void) noexcept
{
    RDGE_FREE(this->regions, nullptr);
    RDGE_FREE(this->animations, nullptr);
}

const texture_part&
SpriteSheet::operator[] (const std::string& name) const
{
    try
    {
        return m_parts.at(name);
    }
    catch (const std::out_of_range& ex)
    {
        std::ostringstream ss;
        ss << "SpriteSheet lookup failed. key=" << name;

        RDGE_THROW(ss.str());
    }
}

const Animation&
SpriteSheet::GetAnimation (const std::string& name) const
{
    try
    {
        return m_animations.at(name);
    }
    catch (const std::out_of_range& ex)
    {
        std::ostringstream ss;
        ss << "SpriteSheet animation lookup failed. key=" << name;

        RDGE_THROW(ss.str());
    }
}

std::unique_ptr<Sprite>
SpriteSheet::CreateSprite (const std::string& name, const math::vec3& pos) const
{
    SDL_assert(this->texture != nullptr);

    const auto& part = (*this)[name]; // Can throw if lookup fails
    return std::make_unique<Sprite>(pos,
                                    static_cast<math::vec2>(part.size),
                                    this->texture,
                                    part.coords);
}

std::unique_ptr<SpriteGroup>
SpriteSheet::CreateSpriteChain (const std::string& name,
                                const math::vec3&  pos,
                                const math::vec2&  to_fill) const
{
    SDL_assert(this->texture != nullptr);

    const auto& part = (*this)[name]; // Can throw if lookup fails
    auto size = static_cast<math::vec2>(part.size);
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
