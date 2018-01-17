#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

Tilemap::Tilemap (const nlohmann::json& j)
{
    try
    {
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

        const auto& j_grid = j["grid"];
        JSON_VALIDATE_REQUIRED(j_grid, renderorder, is_string);
        JSON_VALIDATE_REQUIRED(j_grid, x, is_number);
        JSON_VALIDATE_REQUIRED(j_grid, y, is_number);
        JSON_VALIDATE_REQUIRED(j_grid, width, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_grid, height, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_grid, cells, is_object);
        JSON_VALIDATE_REQUIRED(j_grid, chunks, is_object);
        this->grid.pos.x = j_grid["x"].get<int32>();
        this->grid.pos.y = j_grid["y"].get<int32>();
        this->grid.size.w = j_grid["width"].get<uint32>();
        this->grid.size.h = j_grid["height"].get<uint32>();

        if (!try_parse(j_grid["renderorder"].get<std::string>(), this->grid.render_order))
        {
            throw std::invalid_argument("Tilemap invalid render_order");
        }

        const auto& j_cells = j_grid["cells"];
        JSON_VALIDATE_REQUIRED(j_cells, width, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_cells, height, is_number_unsigned);
        this->grid.cell_size.w = j_cells["width"].get<uint32>();
        this->grid.cell_size.h = j_cells["height"].get<uint32>();

        const auto& j_chunks = j_grid["chunks"];
        JSON_VALIDATE_REQUIRED(j_chunks, width, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_chunks, height, is_number_unsigned);
        this->grid.chunk_size.w = j_chunks["width"].get<uint32>();
        this->grid.chunk_size.h = j_chunks["height"].get<uint32>();

        const auto& j_tilesets = j["tilesets"];
        this->sheets.reserve(j_tilesets.size());
        for (const auto& j_tileset : j_tilesets)
        {
            JSON_VALIDATE_REQUIRED(j_tileset, firstgid, is_number);
            JSON_VALIDATE_REQUIRED(j_tileset, table_id, is_number);
            JSON_VALIDATE_REQUIRED(j_tileset, type, is_number);

            sheet_info sheet;
            sheet.first_gid = j_tileset["firstgid"].get<int32>();
            sheet.table_id = j_tileset["table_id"].get<int32>();
            sheet.type = j_tileset["type"].get<asset_pack::asset_type>();
            this->sheets.push_back(sheet);
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

//TileLayer
//Tilemap::CreateTileLayer (int32 layer_id, float scale)
//{
    //try
    //{
        //const auto& layer = this->layers.at(layer_id);
        //if (layer.type != LayerType::TILELAYER || layer.tileset_index < 0)
        //{
            //throw std::invalid_argument("Invalid TileLayer definition");
        //}

        //const auto& info = this->sheets[layer.tileset_index];
        //auto tileset = g_game.pack->GetTileset(info.table_id);
    //}
    //catch (const std::exception& ex)
    //{
        //RDGE_THROW(ex.what());
    //}
//}

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
