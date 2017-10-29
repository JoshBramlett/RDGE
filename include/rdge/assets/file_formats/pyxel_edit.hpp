//! \headerfile <rdge/assets/file_formats/pyxel_edit.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/json.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \namespace pyxel_edit Pyxel Edit Beta 1.4.4
namespace pyxel_edit {

struct tile
{
    uint32 index = 0;
    uint32 x = 0;
    uint32 y = 0;
    int32 tile = 0;
    bool flipX = false;
    uint32 rot = 0;
};

inline void
to_json (nlohmann::json& j, const tile& t)
{
    j = nlohmann::json {
        { "index", t.index },
        { "x", t.x },
        { "y", t.y },
        { "tile", t.tile },
        { "flipX", t.flipX },
        { "rot", t.rot }
    };
}

inline void
from_json (const nlohmann::json& j, tile& t)
{
    JSON_VALIDATE_REQUIRED(j, index, is_number);
    JSON_VALIDATE_REQUIRED(j, x, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, y, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tile, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, flipX, is_boolean);
    JSON_VALIDATE_REQUIRED(j, rot, is_number);

    t.index = j["index"].get<uint32>();
    t.x = j["x"].get<uint32>();
    t.y = j["y"].get<uint32>();
    t.tile = j["tile"].get<int32>();
    t.flipX = j["flipX"].get<bool>();
    t.rot = j["rot"].get<uint32>();
}

struct layer
{
    std::string name;
    uint32 number = 0;

    std::vector<tile> tiles;
};

inline void
to_json (nlohmann::json& j, const layer& l)
{
    j = nlohmann::json {
        { "name", l.name },
        { "number", l.number },
        { "tiles", l.tiles }
    };
}

inline void
from_json (const nlohmann::json& j, layer& l)
{
    JSON_VALIDATE_REQUIRED(j, name, is_string);
    JSON_VALIDATE_REQUIRED(j, number, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tiles, is_array);

    l.name = j["name"].get<std::string>();
    l.number = j["number"].get<uint32>();
    l.tiles = j["tiles"].get<std::vector<tile>>();
}

struct tilemap
{
    uint32 tileswide = 0;
    uint32 tileshigh = 0;
    uint32 tilewidth = 0;
    uint32 tileheight = 0;

    std::vector<layer> layers;
};

inline void
to_json (nlohmann::json& j, const tilemap& t)
{
    j = nlohmann::json {
        { "tileswide", t.tileswide },
        { "tileshigh", t.tileshigh },
        { "tilewidth", t.tilewidth },
        { "tileheight", t.tileheight },
        { "layers", t.layers }
    };
}

inline void
from_json (const nlohmann::json& j, tilemap& t)
{
    JSON_VALIDATE_REQUIRED(j, tileswide, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tileshigh, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tilewidth, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tileheight, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, layers, is_array);

    t.tileswide = j["tileswide"].get<uint32>();
    t.tileshigh = j["tileshigh"].get<uint32>();
    t.tilewidth = j["tilewidth"].get<uint32>();
    t.tileheight = j["tileheight"].get<uint32>();
    t.layers = j["layers"].get<std::vector<layer>>();
}

} // namespace pyxel_edit
} // namespace rdge

