#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/json.hpp>
#include <rdge/internal/exception_macros.hpp>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

PropertyCollection::PropertyCollection (const nlohmann::json& j)
{
    if (j.count("properties") == 0)
    {
        return;
    }

    try
    {
        const auto& j_properties = j["properties"];
        m_properties.reserve(j_properties.size());
        for (const auto& j_prop : j_properties)
        {
            JSON_VALIDATE_REQUIRED(j_prop, name, is_string);
            JSON_VALIDATE_REQUIRED(j_prop, type, is_string);

            property p;
            p.name = j_prop["name"].get<std::string>();
            p.type = property_type_invalid;

            auto t = j_prop["type"].get<std::string>();
            if      (t == "bool")   { p.type = property_type_bool; }
            else if (t == "color")  { p.type = property_type_color; }
            else if (t == "file")   { p.type = property_type_file; }
            else if (t == "float")  { p.type = property_type_float; }
            else if (t == "int")    { p.type = property_type_int; }
            else if (t == "string") { p.type = property_type_string; }

            switch (p.type)
            {
            case property_type_bool:
                JSON_VALIDATE_REQUIRED(j_prop, value, is_boolean);
                p.i = static_cast<int32>(j_prop["value"].get<bool>());
                break;
            case property_type_float:
                JSON_VALIDATE_REQUIRED(j_prop, value, is_number);
                p.f = j_prop["value"].get<float>();
                break;
            case property_type_int:
                JSON_VALIDATE_REQUIRED(j_prop, value, is_number);
                p.i = j_prop["value"].get<int32>();
                break;
            case property_type_color:
            case property_type_file:
            case property_type_string:
                JSON_VALIDATE_REQUIRED(j_prop, value, is_string);
                p.s = j_prop["value"].get<std::string>();
                break;
            case property_type_invalid:
            default:
                throw std::invalid_argument("PropertyCollection invalid type. key=" + p.name);
            }

            m_properties.push_back(p);
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

std::string
PropertyCollection::GetString (const std::string& name) const
{
    return Lookup(name, property_type_string).s;
}

color
PropertyCollection::GetColor (const std::string& name) const
{
    return color::from_argb(Lookup(name, property_type_color).s);
}

rwops_base
PropertyCollection::GetFile (const std::string& name, const char* mode) const
{
    return rwops_base::from_file(Lookup(name, property_type_file).s.c_str(), mode);
}

float
PropertyCollection::GetFloat (const std::string& name) const
{
    return Lookup(name, property_type_float).f;
}

int32
PropertyCollection::GetInt (const std::string& name) const
{
    return Lookup(name, property_type_int).i;
}

bool
PropertyCollection::GetBool (const std::string& name) const
{
    return static_cast<bool>(Lookup(name, property_type_bool).i);
}

size_t
PropertyCollection::Size (void) const noexcept
{
    return m_properties.size();
}

const PropertyCollection::property&
PropertyCollection::Lookup (const std::string& name, property_type type) const
{
    for (const auto& p : m_properties)
    {
        if (p.name == name)
        {
            if (p.type != type)
            {
                RDGE_THROW("Property type mismatch. key=" + name);
            }

            return p;
        }
    }

    RDGE_THROW("Property lookup failed. key=" + name);
}

} // namespace tilemap
} // namespace rdge
