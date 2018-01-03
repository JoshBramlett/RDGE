#include <rdge/assets/tilemap/property.hpp>
#include <rdge/internal/exception_macros.hpp>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

PropertyCollection::PropertyCollection (const nlohmann::json& j)
{
    // Tiled format
    //
    //"properties":
    //{
     //"cust_prop_bool":true,
     //"cust_prop_color":"#ffec9cc6",
     //"cust_prop_file":"overworld_obj.json",
     //"cust_prop_float":3.14,
     //"cust_prop_int":5,
     //"cust_prop_string":"asdf"
    //},
    //"propertytypes":
    //{
     //"cust_prop_bool":"bool",
     //"cust_prop_color":"color",
     //"cust_prop_file":"file",
     //"cust_prop_float":"float",
     //"cust_prop_int":"int",
     //"cust_prop_string":"string"
    //},

    try
    {
        JSON_VALIDATE_OPTIONAL(j, properties, is_object);
        JSON_VALIDATE_OPTIONAL(j, propertytypes, is_object);

        if (j.find("properties") != j.end())
        {
            const auto& o = j["properties"];
            for (auto it = o.begin(); it != o.end(); ++it)
            {
                property p;
                p.name = it.key();
                p.type = property_type_invalid;

                auto t = j["propertytypes"][p.name].get<std::string>();
                if      (t == "bool")   { p.type = property_type_bool; }
                else if (t == "color")  { p.type = property_type_color; }
                else if (t == "file")   { p.type = property_type_file; }
                else if (t == "float")  { p.type = property_type_float; }
                else if (t == "int")    { p.type = property_type_int; }
                else if (t == "string") { p.type = property_type_string; }

                switch (p.type)
                {
                case property_type_bool:
                    p.i = static_cast<int32>(it.value().get<bool>());
                    break;
                case property_type_float:
                    p.f = it.value().get<float>();
                    break;
                case property_type_int:
                    p.i = it.value().get<int32>();
                    break;
                case property_type_color:
                case property_type_file:
                case property_type_string:
                    p.s = it.value().get<std::string>();
                    break;
                case property_type_invalid:
                default:
                    throw std::invalid_argument("PropertyCollection invalid type. key=" +
                                                p.name);
                }

                m_properties.push_back(p);
            }
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
