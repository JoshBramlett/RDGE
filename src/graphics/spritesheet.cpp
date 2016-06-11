#include <rdge/graphics/spritesheet.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>

namespace RDGE {
namespace Graphics {

using namespace RDGE::Assets;
using namespace RDGE::Math;
using json = nlohmann::json;

SpriteSheet::SpriteSheet (const std::string& config)
{
    try
    {
        auto j = json::parse(config);

        auto texture_file = j["file"];
        if (texture_file == nullptr || texture_file.is_string() == false)
        {
            throw std::invalid_argument("\"file\" key missing from sprite sheet config.");
        }

        auto uv_array = j["uv"];
        if (uv_array == nullptr || uv_array.is_array() == false)
        {
            throw std::invalid_argument("\"uv\" key missing from sprite sheet config.");
        }

        for (const auto& uv_element : uv_array)
        {
            auto index = uv_element["index"];
            auto name = uv_element["name"];
            auto values = uv_element["values"];

            if (
                uv_element.is_object() == false ||
                index == nullptr || index.is_number() == false ||
                name == nullptr || name.is_string() == false ||
                values == nullptr || values.is_array() == false || values.size() != 4
               )
            {
                std::stringstream ss;
                ss << "Invalid uv value. element=" << uv_element;

                throw ss.str();
            }

            sheet_element se;
            se.index = static_cast<RDGE::Int32>(index);
            se.name = name;
            for (const auto& v : values)
            {
                if (v.is_array() == false || v.size() != 2)
                {
                    std::stringstream ss;
                    ss << "Invalid uv value. element=" << uv_element;

                    throw ss.str();
                }

                se.uv.push_back(vec2(static_cast<float>(v[0]), static_cast<float>(v[1])));
            }

            m_elements.push_back(se);
        }

        auto surface = std::make_shared<Surface>(texture_file.get<std::string>());
        m_textureId = OpenGL::CreateTexture();
        ResetData(surface);
    }
    catch (const std::domain_error& ex)
    {
        std::stringstream ss;
        ss << "Domain error parsing sprite sheet config."
           << " error=" << ex.what();

        RDGE_THROW(ss.str());
    }
    catch (const std::out_of_range& ex)
    {
        std::stringstream ss;
        ss << "Out of range error parsing sprite sheet config."
           << " error=" << ex.what();

        RDGE_THROW(ss.str());
    }
    catch (const std::invalid_argument& ex)
    {
        RDGE_THROW(ex.what());
    }
}

const UVCoordinates&
SpriteSheet::LookupUV (RDGE::Int32 index)
{
    auto it = std::find_if(
                           m_elements.begin(), m_elements.end(),
                           [index] (const sheet_element& e) { return e.index == index; }
                          );
    if (it == m_elements.end())
    {
        RDGE_THROW("Sprite sheet element not found.  index=" + std::to_string(index));
    }

    return (*it).uv;
}

const UVCoordinates&
SpriteSheet::LookupUV (const std::string& name)
{
    auto it = std::find_if(
                           m_elements.begin(), m_elements.end(),
                           [name] (const sheet_element& e) { return e.name == name; }
                          );
    if (it == m_elements.end())
    {
        RDGE_THROW("Sprite sheet element not found.  name=" + name);
    }

    return (*it).uv;
}

} // namespace Graphics
} // namespace RDGE
