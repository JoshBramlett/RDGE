//! \headerfile <rdge/assets/file_formats/tiled.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/json.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \namespace tiled Tiled Map Editor v1.0.3
namespace tiled {

struct layer
{
    std::string name;
    std::string type = "tilelayer";
    uint32 x = 0;
    uint32 y = 0;
    uint32 width = 0;
    uint32 height = 0;
    float opacity = 1.f;
    bool visible = true;
    uint32 offsetx = 0;
    uint32 offsety = 0;

    std::vector<uint32> data;
};

inline void
to_json (nlohmann::json& j, const layer& l)
{
    j = nlohmann::json {
        { "name", l.name },
        { "type", l.type },
        { "x", l.x },
        { "y", l.y },
        { "width", l.width },
        { "height", l.height },
        { "opacity", l.opacity },
        { "visible", l.visible },
        { "offsetx", l.offsetx },
        { "offsety", l.offsety },
        { "data", l.data }
    };
}

inline void
from_json (const nlohmann::json& j, layer& l)
{
    l.name = j["name"].get<std::string>();
    l.type = j["type"].get<std::string>();
    l.x = j["x"].get<uint32>();
    l.y = j["y"].get<uint32>();
    l.width = j["width"].get<uint32>();
    l.height = j["height"].get<uint32>();
    l.opacity = j["opacity"].get<float>();
    l.visible = j["visible"].get<bool>();
    l.offsetx = j["offsetx"].get<uint32>();
    l.offsety = j["offsety"].get<uint32>();
    l.data = j["data"].get<std::vector<uint32>>();
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
