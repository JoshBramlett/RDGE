#include <rdge/assets/spritesheet.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>

using json = nlohmann::json;

namespace {

using namespace rdge;

// Normalize the point to a float clamped to [0, 1]
constexpr float32 normalize (uint64 point, uint64 dimension)
{
    return static_cast<float32>(point) / static_cast<float32>(dimension);
}

} // anonymous namespace

namespace rdge {

SpriteSheet::SpriteSheet (const std::string& path, bool scale_for_hires)
{
    try
    {
        auto config = rdge::util::read_text_file(path.c_str());
        if (config.empty())
        {
            throw std::invalid_argument("File does not exist.");
        }

        auto j = json::parse(config);
        auto image_path = j["image_path"].get<std::string>();

        this->surface = std::make_shared<Surface>(image_path);  // Will throw if failed
        this->texture = std::make_shared<Texture>(*this->surface);
        auto surface_size = this->surface->Size();

        auto texture_parts = j["texture_parts"];
        for (const auto& part : texture_parts)
        {
            // Validate for unsigned to prevent overflow issues
            if (!part["x"].is_number_unsigned() || !part["y"].is_number_unsigned() ||
                !part["width"].is_number_unsigned() || !part["height"].is_number_unsigned())
            {
                throw std::invalid_argument("Parts must contain unsigned integers");
            }

            auto name = part["name"].get<std::string>();
            auto x    = part["x"].get<uint32>();
            auto y    = part["y"].get<uint32>();
            auto w    = part["width"].get<uint32>();
            auto h    = part["height"].get<uint32>();

            // Validate values are within range
            if ((x + w > surface_size.w) || (y + h > surface_size.h))
            {
                throw std::invalid_argument("Part has invalid dimensions");
            }

            uint32 scale = scale_for_hires ? 2 : 1;
            texture_part result;
            result.name = name;
            result.size = { w * scale, h * scale };
            result.coords.bottom_left  = math::vec2(normalize(x, surface_size.w),
                                                    normalize(y, surface_size.h));
            result.coords.bottom_right = math::vec2(normalize(x + w, surface_size.w),
                                                    normalize(y, surface_size.h));
            result.coords.top_left     = math::vec2(normalize(x, surface_size.w),
                                                    normalize(y + h, surface_size.h));
            result.coords.top_right    = math::vec2(normalize(x + w, surface_size.w),
                                                    normalize(y + h, surface_size.h));

            auto emplaced = m_parts.emplace(std::make_pair(name, result));
            if (emplaced.second == false)
            {
                std::ostringstream ss;
                ss << "Part could not be added to the collection. "
                   << "check to see if there are duplicate keys. "
                   << "key=" << emplaced.first->first;

                throw std::invalid_argument(ss.str());
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
