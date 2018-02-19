//! \headerfile <rdge/assets/file_formats/tiled.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/util/json.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \namespace tiled Tiled Map Editor v1.1.0
namespace tiled {

// Holy fuck is this poorly implemented.  Seriously, wtf guys?
//
//  - Values and types are separated and require name lookups to resolve,
//    meaning I cannot use standard to_json/from_json serialization methods.
//
//  - They're represented as key/value pairs rather than an array, so
//    there's basically no way to represent that in c++.

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
//

// Would this have been so hard?
//
// "properties": [
//   {
//     "name": "fuck you",
//     "type": "bool",
//     "value": true
//   }
// ]

enum property_type
{
    property_type_bool,
    property_type_color,
    property_type_file,
    property_type_float,
    property_type_int,
    property_type_string
};

std::string to_string (property_type);
bool from_string (const std::string&, property_type&);

struct property
{
    std::string name;
    property_type type;

    // TODO Replace with std::variant once Apple gets their head out of
    //      their asses and packages it.
    // NOTE Opted to not union the fields b/c strings are a hassle.
    bool        bool_value;
    float       float_value;
    int32       int_value;
    std::string string_value;
};

inline void
serialize (nlohmann::json& j, const std::vector<property>& plist)
{
    if (plist.empty())
    {
        return;
    }

    j = nlohmann::json {
        { "properties", nlohmann::json::object() },
        { "propertytypes", nlohmann::json::object() }
    };

    auto& j_props = j["properties"];
    auto& j_types = j["propertytypes"];
    for (const auto& p : plist)
    {
        j_types[p.name] = to_string(p.type);

        switch (p.type)
        {
        case property_type_bool:
            j_props[p.name] = p.bool_value;
            break;
        case property_type_float:
            j_props[p.name] = p.float_value;
            break;
        case property_type_int:
            j_props[p.name] = p.int_value;
            break;
        case property_type_color:
        case property_type_file:
        case property_type_string:
            j_props[p.name] = p.string_value;
            break;
        }
    }
}

inline void
deserialize (const nlohmann::json& j, std::vector<property>& plist)
{
    const auto& o = j["properties"];
    for (auto it = o.begin(); it != o.end(); ++it)
    {
        property p;
        p.name = it.key();
        from_string(j["propertytypes"][p.name].get<std::string>(), p.type);

        switch (p.type)
        {
        case property_type_bool:
            p.bool_value = j["properties"][p.name].get<bool>();
            break;
        case property_type_float:
            p.float_value = j["properties"][p.name].get<float>();
            break;
        case property_type_int:
            p.int_value = j["properties"][p.name].get<int32>();
            break;
        case property_type_color:
        case property_type_file:
        case property_type_string:
            p.string_value = j["properties"][p.name].get<std::string>();
            break;
        }

        plist.push_back(p);
    }
}

//! \enum object_type
//! \brief Interally used only
//! \details Used to track the object type because the way Tiled determines
//!          it is utter garbage.  They check for the existence of fields
//!          (i.e. point, ellipse, polygon, etc.) which makes serialization
//!          stupidly more difficult.
enum class object_type
{
    sprite,
    rect,
    point,
    ellipse,
    polygon,
    polyline,
    text
};

struct coordinate
{
    int32 x;
    int32 y;
};

inline void
to_json (nlohmann::json& j, const coordinate& c)
{
    j = nlohmann::json {
        { "x", c.x },
        { "y", c.y }
    };
}

inline void
from_json (const nlohmann::json& j, coordinate& c)
{
    JSON_VALIDATE_REQUIRED(j, x, is_number);
    JSON_VALIDATE_REQUIRED(j, y, is_number);

    c.x = j["x"].get<int32>();
    c.y = j["y"].get<int32>();
}

struct object_text
{
    std::string text;
    bool wrap;
};

inline void
to_json (nlohmann::json& j, const object_text& t)
{
    j = nlohmann::json {
        { "text", t.text },
        { "wrap", t.wrap }
    };
}

inline void
from_json (const nlohmann::json& j, object_text& t)
{
    JSON_VALIDATE_REQUIRED(j, text, is_string);
    JSON_VALIDATE_REQUIRED(j, wrap, is_boolean);

    t.text = j["text"].get<std::string>();
    t.wrap = j["wrap"].get<bool>();
}

struct object
{
    //!@{ Required
    int32 id;         //!< Unique id
    std::string name; //!< String assigned to name field in editor
    std::string type; //!< String assigned to type field in editor
    int32 x;          //!< x-coordinate in pixels
    int32 y;          //!< y-coordinate in pixels
    int32 width;      //!< Width in pixels (ignored if using a gid)
    int32 height;     //!< Height in pixels (ignored if using a gid)
    bool visible;     //!< Whether object is shown in editor
    float rotation;   //!< Angle in degrees clockwise
    //!@}

    //!@{ Optional
    int32 gid = 0;                    //!< GID, only if object comes from a Tilemap
    std::vector<property> properties; //!< Custom properties
    //!@}

    //!@{ Object type
    object_type otype = object_type::rect; //!< Internal
    std::vector<coordinate> coords;        //!< Coordinate list in pixels
    object_text text;                      //!< String key/value pairs

    // NOTE: Original implementation
    //bool point;                       //!< Used to mark an object as a point
    //bool ellipse;                     //!< Used to mark an object as an ellipse
    //std::vector<coordinate> polygon;  //!< Coordinate list in pixels
    //std::vector<coordinate> polyline; //!< Coordinate list in pixels
    //object_text text;                 //!< String key/value pairs

    //!@}
};

inline void
to_json (nlohmann::json& j, const object& o)
{
    j = nlohmann::json {
        { "id", o.id },
        { "name", o.name },
        { "type", o.type },
        { "x", o.x },
        { "y", o.y },
        { "width", o.width },
        { "height", o.height },
        { "visible", o.visible },
        { "rotation", o.rotation }
    };

    serialize(j, o.properties);

    switch (o.otype)
    {
    case object_type::sprite:
        j["gid"] = o.gid;
    case object_type::rect:
        break;
    case object_type::point:
        j["point"] = true;
        break;
    case object_type::ellipse:
        j["ellipse"] = true;
        break;
    case object_type::polygon:
        j["polygon"] = o.coords;
        break;
    case object_type::polyline:
        j["polyline"] = o.coords;
        break;
    case object_type::text:
        j["text"] = o.text;
        break;
    default:
        throw std::invalid_argument("serializing tiled object with no type");
    }
}

inline void
from_json (const nlohmann::json& j, object& o)
{
    JSON_VALIDATE_REQUIRED(j, id, is_number);
    JSON_VALIDATE_REQUIRED(j, name, is_string);
    JSON_VALIDATE_REQUIRED(j, type, is_string);
    JSON_VALIDATE_REQUIRED(j, x, is_number);
    JSON_VALIDATE_REQUIRED(j, y, is_number);
    JSON_VALIDATE_REQUIRED(j, width, is_number);
    JSON_VALIDATE_REQUIRED(j, height, is_number);
    JSON_VALIDATE_REQUIRED(j, visible, is_boolean);
    JSON_VALIDATE_REQUIRED(j, rotation, is_number_float);

    JSON_VALIDATE_OPTIONAL(j, properties, is_object);
    JSON_VALIDATE_OPTIONAL(j, propertytypes, is_object);

    JSON_VALIDATE_OPTIONAL(j, gid, is_number);
    JSON_VALIDATE_OPTIONAL(j, point, is_boolean);
    JSON_VALIDATE_OPTIONAL(j, ellipse, is_boolean);
    JSON_VALIDATE_OPTIONAL(j, polygon, is_array);
    JSON_VALIDATE_OPTIONAL(j, polyline, is_array);
    JSON_VALIDATE_OPTIONAL(j, text, is_string);

    o.id = j["id"].get<int32>();
    o.name = j["name"].get<std::string>();
    o.type = j["type"].get<std::string>();
    o.x = j["x"].get<int32>();
    o.y = j["y"].get<int32>();
    o.width = j["width"].get<int32>();
    o.height = j["height"].get<int32>();
    o.visible = j["visible"].get<bool>();
    o.rotation = j["rotation"].get<float>();

    deserialize(j, o.properties);

    if (j.count("gid"))
    {
        o.gid = j["gid"].get<int32>();
        if (o.gid <= 0)
        {
            throw std::invalid_argument("object has invalid gid");
        }

        o.otype = object_type::sprite;
        return;
    }

    o.otype = object_type::rect;
    if (j.count("point") && j["point"].get<bool>())
    {
        o.otype = object_type::point;
    }

    if (j.count("ellipse") && j["ellipse"].get<bool>())
    {
        o.otype = object_type::ellipse;
    }

    if (j.count("polygon"))
    {
        o.otype = object_type::polygon;
        o.coords = j["polygon"].get<std::vector<coordinate>>();
    }

    if (j.count("polyline"))
    {
        o.otype = object_type::polyline;
        o.coords = j["polyline"].get<std::vector<coordinate>>();
    }

    if (j.count("text"))
    {
        o.otype = object_type::text;
        o.text = j["text"].get<object_text>();
    }
}

enum class layer_type
{
    tilelayer,
    objectgroup,
    imagelayer
};

std::string to_string (layer_type);
bool from_string (const std::string&, layer_type&);

enum class layer_draworder
{
    topdown, //!< Objects drawn sorted by y-coordinate
    index    //!< Objects drawn in order of appearance
};

std::string to_string (layer_draworder order);
bool from_string (const std::string& test, layer_draworder& out);

struct layer
{
    //!@{ Required
    std::string name;    //!< Name assigned to this layer
    layer_type type;     //!<
    bool visible = true; //!< Whether layer is shown or hidden in editor
    float opacity = 1.f; //!< Layer opacity (normalized)
    int32 x = 0;         //!< Horizontal layer offset in tiles (always zero)
    int32 y = 0;         //!< Vertical layer offset in tiles (always zero)
    //!@}

    //!@{ Optional
    float offsetx = 0.f;              //!< Rendering x-offset in pixels
    float offsety = 0.f;              //!< Rendering y-offset in pixels
    std::vector<property> properties; //!< Custom properties
    //!@}

    //!@{ type == tilelayer
    std::vector<uint32> data; //!< Array of GIDs
    int32 width = 0;          //!< Column count
    int32 height = 0;         //!< Row count
    //!@}

    //!@{ type == objectgroup
    std::vector<object> objects; //!< Array of objects
    layer_draworder draworder;   //!< Order objects are drawn
    //!@}

    //!@{ type == imagelayer
    std::string image; //!< path to image
    //!@}
};

inline void
to_json (nlohmann::json& j, const layer& l)
{
    j = nlohmann::json {
        { "name", l.name },
        { "type", to_string(l.type) },
        { "visible", l.visible },
        { "opacity", l.opacity },
        { "x", l.x },
        { "y", l.y }
    };

    if (l.offsetx != 0.f || l.offsety != 0.f)
    {
        j["offsetx"] = l.offsetx;
        j["offsety"] = l.offsety;
    }

    serialize(j, l.properties);

    switch (l.type)
    {
    case layer_type::tilelayer:
        j["width"] = l.width;
        j["height"] = l.height;
        j["data"] = l.data;
        break;
    case layer_type::objectgroup:
        j["objects"] = l.objects;
        j["draworder"] = to_string(l.draworder);
        break;
    case layer_type::imagelayer:
        j["image"] = l.image;
        break;
    }
}

inline void
from_json (const nlohmann::json& j, layer& l)
{
    //l.type = j["type"].get<std::string>();
    from_string(j["type"].get<std::string>(), l.type);

    l.name = j["name"].get<std::string>();
    l.x = j["x"].get<uint32>();
    l.y = j["y"].get<uint32>();
    l.width = j["width"].get<uint32>();
    l.height = j["height"].get<uint32>();
    l.opacity = j["opacity"].get<float>();
    l.visible = j["visible"].get<bool>();
    l.offsetx = j["offsetx"].get<uint32>();
    l.offsety = j["offsety"].get<uint32>();
    l.data = j["data"].get<std::vector<uint32>>();

    deserialize(j, l.properties);
}

struct tileset
{
    enum tiled_tileset_type
    {
        tiled_tileset_type_external,
        tiled_tileset_type_embedded
    };

    // all types
    uint32 firstgid = 1;

    // tiled_tileset_type_external
    std::string source;

    // tiled_tileset_type_embedded
    std::string name;
    uint32 tilewidth = 0;
    uint32 tileheight = 0;
    uint32 spacing = 0;
    uint32 margin = 0;
    uint32 tilecount = 0;
    uint32 columns = 0;
    std::string image;
    uint32 imagewidth = 0;
    uint32 imageheight = 0;

    tiled_tileset_type type = tiled_tileset_type_external;
};

inline void
to_json (nlohmann::json& j, const tileset& ts)
{
    if (ts.type == tileset::tiled_tileset_type_external)
    {
        j = nlohmann::json {
            { "firstgid", ts.firstgid },
            { "source", ts.source }
        };
    }
    else if (ts.type == tileset::tiled_tileset_type_embedded)
    {
        j = nlohmann::json {
            { "firstgid", ts.firstgid },
            { "name", ts.name },
            { "tilewidth", ts.tilewidth },
            { "tileheight", ts.tileheight },
            { "spacing", ts.spacing },
            { "margin", ts.margin },
            { "tilecount", ts.tilecount },
            { "columns", ts.columns },
            { "image", ts.image },
            { "imagewidth", ts.imagewidth },
            { "imageheight", ts.imageheight }
        };
    }
}

inline void
from_json (const nlohmann::json& j, tileset& ts)
{
    ts.firstgid = j["firstgid"].get<uint32>();

    if (j.count("source"))
    {
        ts.type = tileset::tiled_tileset_type_external;
        ts.source = j["source"].get<std::string>();
    }
    else
    {
        ts.type = tileset::tiled_tileset_type_embedded;
        ts.name = j["name"].get<std::string>();
        ts.tilewidth = j["tilewidth"].get<uint32>();
        ts.tileheight = j["tileheight"].get<uint32>();
        ts.spacing = j["spacing"].get<uint32>();
        ts.margin = j["margin"].get<uint32>();
        ts.tilecount = j["tilecount"].get<uint32>();
        ts.columns = j["columns"].get<uint32>();
        ts.image = j["image"].get<std::string>();
        ts.imagewidth = j["imagewidth"].get<uint32>();
        ts.imageheight = j["imageheight"].get<uint32>();
    }
}

struct tilemap
{
    std::string type = "map";
    uint32 version = 1;
    std::string tiledversion = "1.0.3";
    std::string orientation = "orthogonal";
    std::string renderorder = "right-down";

    bool infinite = false;
    uint32 width = 0;
    uint32 height = 0;
    uint32 tilewidth = 0;
    uint32 tileheight = 0;
    uint32 nextobjectid = 1;

    std::vector<layer> layers;
    std::vector<tileset> tilesets;
};

inline void
to_json (nlohmann::json& j, const tilemap& t)
{
    j = nlohmann::json {
        { "type", t.type },
        { "version", t.version },
        { "tiledversion", t.tiledversion },
        { "orientation", t.orientation },
        { "renderorder", t.renderorder },
        { "infinite", t.infinite },
        { "width", t.width },
        { "height", t.height },
        { "tilewidth", t.tilewidth },
        { "tileheight", t.tileheight },
        { "nextobjectid", t.nextobjectid },
        { "layers", t.layers },
        { "tilesets", t.tilesets }
    };
}

inline void
from_json (const nlohmann::json& j, tilemap& t)
{
    t.type = j["type"].get<std::string>();
    t.version = j["version"].get<uint32>();
    t.tiledversion = j["tiledversion"].get<std::string>();
    t.orientation = j["orientation"].get<std::string>();
    t.renderorder = j["renderorder"].get<std::string>();
    t.infinite = j["infinite"].get<bool>();
    t.width = j["width"].get<uint32>();
    t.height = j["height"].get<uint32>();
    t.tilewidth = j["tilewidth"].get<uint32>();
    t.tileheight = j["tileheight"].get<uint32>();
    t.nextobjectid = j["nextobjectid"].get<uint32>();
    t.layers = j["layers"].get<std::vector<layer>>();
    t.tilesets = j["tilesets"].get<std::vector<tileset>>();
}

} // namespace tiled
} // namespace rdge
