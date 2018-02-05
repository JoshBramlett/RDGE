#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {

void
from_json (const nlohmann::json& j, tilemap_grid& grid)
{
    JSON_VALIDATE_REQUIRED(j, renderorder, is_string);
    JSON_VALIDATE_REQUIRED(j, x, is_number);
    JSON_VALIDATE_REQUIRED(j, y, is_number);
    JSON_VALIDATE_REQUIRED(j, width, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, height, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, cells, is_object);
    JSON_VALIDATE_REQUIRED(j, chunks, is_object);

    grid.pos.x = j["x"].get<decltype(grid.pos.x)>();
    grid.pos.y = j["y"].get<decltype(grid.pos.x)>();
    grid.size.w = j["width"].get<decltype(grid.size.w)>();
    grid.size.h = j["height"].get<decltype(grid.size.h)>();

    if (!try_parse(j["renderorder"].get<std::string>(), grid.render_order))
    {
        throw std::invalid_argument("Tilemap invalid render_order");
    }

    const auto& j_cells = j["cells"];
    JSON_VALIDATE_REQUIRED(j_cells, width, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j_cells, height, is_number_unsigned);
    grid.cell_size.w = j_cells["width"].get<decltype(grid.cell_size.w)>();
    grid.cell_size.h = j_cells["height"].get<decltype(grid.cell_size.h)>();

    const auto& j_chunks = j["chunks"];
    JSON_VALIDATE_REQUIRED(j_chunks, width, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j_chunks, height, is_number_unsigned);
    grid.chunk_size.w = j_chunks["width"].get<decltype(grid.chunk_size.w)>();
    grid.chunk_size.h = j_chunks["height"].get<decltype(grid.chunk_size.w)>();
}

namespace tilemap {

using json = nlohmann::json;

Tilemap::Tilemap (const std::vector<uint8>& msgpack, PackFile& packfile)
{
    try
    {
        json j = json::from_msgpack(msgpack);
        JSON_VALIDATE_REQUIRED(j, orientation, is_string);
        JSON_VALIDATE_REQUIRED(j, grid, is_object);
        JSON_VALIDATE_REQUIRED(j, layers, is_array);
        JSON_VALIDATE_REQUIRED(j, tilesets, is_array);

        JSON_VALIDATE_OPTIONAL(j, backgroundcolor, is_string);

        // required
        if (!try_parse(j["orientation"].get<std::string>(), this->orientation))
        {
            throw std::invalid_argument("Tilemap invalid orientation");
        }

        if (this->orientation != Orientation::ORTHOGONAL)
        {
            throw std::invalid_argument("Tilemap only supports orthogonal maps");
        }

        this->grid = j["grid"].get<tilemap_grid>();

        const auto& j_tilesets = j["tilesets"];
        std::vector<sheet_info>(j_tilesets.size()).swap(this->sheets);

        size_t index = 0;
        for (const auto& j_tileset : j_tilesets)
        {
            JSON_VALIDATE_REQUIRED(j_tileset, firstgid, is_number);
            JSON_VALIDATE_REQUIRED(j_tileset, table_id, is_number);
            JSON_VALIDATE_REQUIRED(j_tileset, type, is_number);

            auto& sheet = this->sheets.at(index++);
            sheet.first_gid = j_tileset["firstgid"].get<decltype(sheet.first_gid)>();
            sheet.table_id = j_tileset["table_id"].get<decltype(sheet.table_id)>();
            sheet.type = j_tileset["type"].get<decltype(sheet.type)>();

            if (sheet.type == asset_pack::asset_type_spritesheet)
            {
                sheet.spritesheet = packfile.GetAsset<SpriteSheet>(sheet.table_id);
            }
            else if (sheet.type == asset_pack::asset_type_tileset)
            {
                sheet.tileset = packfile.GetAsset<Tileset>(sheet.table_id);
            }
        }

        for (auto& j_layer : j["layers"])
        {
            this->layers.emplace_back(j_layer);
        }

        // optional
        this->properties = PropertyCollection(j);

        if (j.count("backgroundcolor"))
        {
            this->background = color::from_argb(j["backgroundcolor"].get<std::string>());
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

TileLayer
Tilemap::CreateTileLayer (int32 layer_id, float scale)
{
    try
    {
        const auto& layer = this->layers.at(layer_id);
        if (layer.type != LayerType::TILELAYER || layer.tileset_index < 0)
        {
            throw std::invalid_argument("Invalid TileLayer definition");
        }

        const auto& sheet = this->sheets[layer.tileset_index];
        if (sheet.type != asset_pack::asset_type_tileset)
        {
            throw std::invalid_argument("TileLayer not mapped to Tileset");
        }

        return TileLayer(grid, layer, *sheet.tileset, scale);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

NewSpriteLayer
Tilemap::CreateSpriteLayer (int32 layer_id, float scale)
{
    try
    {
        const auto& layer = this->layers.at(layer_id);
        if (layer.type != LayerType::OBJECTGROUP || layer.tileset_index < 0)
        {
            throw std::invalid_argument("Invalid SpriteLayer definition");
        }

        const auto& sheet = this->sheets[layer.tileset_index];
        if (sheet.type != asset_pack::asset_type_spritesheet)
        {
            throw std::invalid_argument("TileLayer not mapped to SpriteSheet");
        }

        return NewSpriteLayer(layer, *sheet.spritesheet, scale);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

std::ostream&
operator<< (std::ostream& os, Orientation value)
{
    return os << rdge::to_string(value);
}

} // namespace tilemap

std::string
to_string (tilemap::Orientation value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(tilemap::Orientation::INVALID)
        CASE(tilemap::Orientation::ORTHOGONAL)
        CASE(tilemap::Orientation::ISOMETRIC)
        CASE(tilemap::Orientation::STAGGERED)
        CASE(tilemap::Orientation::HEXAGONAL)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& s, tilemap::Orientation& out)
{
    if      (s == "orthogonal") { out = tilemap::Orientation::ORTHOGONAL; return true; }
    else if (s == "isometric")  { out = tilemap::Orientation::ISOMETRIC;  return true; }
    else if (s == "staggered")  { out = tilemap::Orientation::STAGGERED;  return true; }
    else if (s == "hexagonal")  { out = tilemap::Orientation::HEXAGONAL;  return true; }

    return false;
}

} // namespace rdge
