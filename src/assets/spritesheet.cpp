#include <rdge/assets/spritesheet.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>
#include <cstdint>

using namespace rdge::assets;
using namespace rdge::math;
using json = nlohmann::json;

// TODO Unsure if I like this implementation
//
// It requires the implementation know the names beforehand.  Meaning the
// lookup is limited to:
//
// auto uv = spritesheet["my_sprite"];
//
// Alternatives could be:
// 1) Just have a function return a deserialized version of everything.  For
//    example, define a struct that contains a surface, and a vector of the uv data.
// 2) Return the map?  Or provide another method to get the collection to iterature
//    through?
// 3) Can I construct all the sprites and the texture and provide that?
// 4) Have the ability to pass the spritesheet to the layer and have that do all
//    the processing?

namespace {

    // converts a pixel to a uv value, where p is the pixel (x or y axis) and
    // dim is the corresponding width/height.  Params are type std::uint64_t
    // because that's how the nlohmann::json lib represents them
    constexpr float to_uv (std::uint64_t p, std::uint64_t dim)
    {
        return static_cast<float>((p-1) / dim);
    }

} // anonymous namespace

SpriteSheet::SpriteSheet (const std::string& path)
{
    try
    {
        auto config = rdge::util::read_text_file(path.c_str());
        if (config.empty())
        {
            throw std::invalid_argument("File does not exist.");
        }

        auto j = json::parse(config);

        // 1) Validate "file" string
        auto texture_file = j["file"];
        if (!texture_file.is_string())
        {
            throw std::invalid_argument("\"file\" key missing from SpriteSheet config.");
        }

        // 2) Validate "uv" array
        auto uv_array = j["uv"];
        if (!uv_array.is_array())
        {
            throw std::invalid_argument("\"uv\" key missing from SpriteSheet config.");
        }

        // Will throw if failed
        m_surface = std::make_shared<Surface>(texture_file.get<std::string>());
        auto surface_size = m_surface->Size();

        for (const auto& uv : uv_array)
        {
            // 3) Validate element of "uv" array is an object
            if (!uv.is_object())
            {
                throw std::invalid_argument("uv element is not an object");
            }

            auto name = uv["name"];
            auto pos  = uv["pos"];
            auto size = uv["size"];

            // 4) Validate types and sizes of the element
            if (!name.is_string() ||
                !pos.is_array() || pos.size() != 2 ||
                !size.is_array() || size.size() != 2)
            {
                std::ostringstream ss;
                ss << "uv element has an invalid format. element=" << uv;

                throw std::invalid_argument(ss.str());
            }

            auto x = pos[0].get<std::uint64_t>();
            auto y = pos[1].get<std::uint64_t>();
            auto w = size[0].get<std::uint64_t>();
            auto h = size[1].get<std::uint64_t>();

            // 5) Validate position/size values
            //    FIXME Could potentially add checks against a signed type for negative
            //          values and check against overflow
            if (x > surface_size.w || y > surface_size.h ||
                w > surface_size.w || h > surface_size.h)
            {
                std::ostringstream ss;
                ss << "uv element has values that exceed the surface size. element=" << uv;

                throw std::invalid_argument(ss.str());
            }

            rdge::gfx::sprite_uv result;
            result.top_left     = rdge::math::vec2(to_uv(x, surface_size.w),
                                                   to_uv(y, surface_size.h));
            result.top_right    = rdge::math::vec2(to_uv(x + w, surface_size.w),
                                                   to_uv(y, surface_size.h));
            result.bottom_left  = rdge::math::vec2(to_uv(x, surface_size.w),
                                                   to_uv(y + h, surface_size.h));
            result.bottom_right = rdge::math::vec2(to_uv(x + w, surface_size.w),
                                                   to_uv(y + h, surface_size.h));

            m_elements.emplace(std::make_pair(name.get<std::string>(), result));
        }
    }
    catch (const std::domain_error& ex)
    {
        std::ostringstream ss;
        ss << "Exception parsing SpriteSheet config."
           << " type=domain_error"
           << " what=" << ex.what();

        RDGE_THROW(ss.str());
    }
    catch (const std::out_of_range& ex)
    {
        std::ostringstream ss;
        ss << "Exception parsing SpriteSheet config."
           << " type=out_of_range"
           << " what=" << ex.what();

        RDGE_THROW(ss.str());
    }
    catch (const std::invalid_argument& ex)
    {
        std::ostringstream ss;
        ss << "Exception parsing SpriteSheet config."
           << " type=invalid_argument"
           << " what=" << ex.what();

        RDGE_THROW(ss.str());
    }
    catch (...)
    {
        RDGE_THROW("Exception parsing SpriteSheet config");
    }
}

rdge::gfx::sprite_uv
SpriteSheet::operator[] (const std::string& name)
{
    // TODO stopgap
    return m_elements[name];
}

std::shared_ptr<rdge::Surface>
SpriteSheet::GetSurface (void) const
{
    return m_surface;
}
