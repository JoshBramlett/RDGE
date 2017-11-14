#include <rdge/assets/file_formats/tiled.hpp>

namespace rdge {
namespace tiled {

std::string
to_string (property_type val)
{
    static constexpr size_t offset = sizeof("property_type_") - 1;

    switch (val)
    {
#define CASE(X) case X: return (#X + offset);
        CASE(property_type_bool)
        CASE(property_type_color)
        CASE(property_type_file)
        CASE(property_type_float)
        CASE(property_type_int)
        CASE(property_type_string)
        default: break;
#undef CASE
    }

    return "unknown";
}

bool
from_string (const std::string& test, property_type& out)
{
    if (test == "bool")
    {
        out = property_type_bool;
        return true;
    }
    else if (test == "color")
    {
        out = property_type_color;
        return true;
    }
    else if (test == "file")
    {
        out = property_type_file;
        return true;
    }
    else if (test == "float")
    {
        out = property_type_float;
        return true;
    }
    else if (test == "int")
    {
        out = property_type_int;
        return true;
    }
    else if (test == "string")
    {
        out = property_type_string;
        return true;
    }

    return false;
}

std::string
to_string (layer_type val)
{
    switch (val)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(layer_type::tilelayer)
        CASE(layer_type::objectgroup)
        CASE(layer_type::imagelayer)
#undef CASE
        default:
            break;
    }

    return "unknown";
}

bool
from_string (const std::string& test, layer_type& out)
{
    if (test == "tilelayer")
    {
        out = layer_type::tilelayer;
        return true;
    }
    else if (test == "objectgroup")
    {
        out = layer_type::objectgroup;
        return true;
    }
    else if (test == "imagelayer")
    {
        out = layer_type::imagelayer;
        return true;
    }

    return false;
}

std::string
to_string (layer_draworder val)
{
    switch (val)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(layer_draworder::topdown)
        CASE(layer_draworder::index)
#undef CASE
        default:
            break;
    }

    return "unknown";
}

bool
from_string (const std::string& test, layer_draworder& out)
{
    if (test == "topdown")
    {
        out = layer_draworder::topdown;
        return true;
    }
    else if (test == "index")
    {
        out = layer_draworder::index;
        return true;
    }

    return false;
}

} // namespace tiled
} // namespace rdge
