#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/tileset.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/strings.hpp>

#include <sstream>
#include <cstring> // strrchr

using json = nlohmann::json;

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

void
from_json (const nlohmann::json& j, Tilemap::sheet_info& sheet)
{
    JSON_VALIDATE_REQUIRED(j, firstgid, is_number);
    JSON_VALIDATE_REQUIRED(j, table_id, is_number);
    JSON_VALIDATE_REQUIRED(j, type, is_number);

    sheet.first_gid = j["firstgid"].get<decltype(sheet.first_gid)>();
    sheet.table_id = j["table_id"].get<decltype(sheet.table_id)>();
    sheet.type = j["type"].get<decltype(sheet.type)>();
}

void
from_json (const nlohmann::json& j, extended_object_data& data)
{
    JSON_VALIDATE_REQUIRED(j, name, is_string);
    JSON_VALIDATE_REQUIRED(j, color, is_string);
    JSON_VALIDATE_REQUIRED(j, properties, is_array);

    data.name = j["name"].get<decltype(data.name)>();
    data.color = color::from_argb(j["color"].get<std::string>());
    data.properties = PropertyCollection(j);
}

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
        JSON_VALIDATE_OPTIONAL(j, object_types, is_array);

        // required
        if (!try_parse(j["orientation"].get<std::string>(), this->orientation))
        {
            throw std::invalid_argument("Tilemap invalid orientation");
        }

        if (RDGE_UNLIKELY(this->orientation != Orientation::ORTHOGONAL))
        {
            throw std::invalid_argument("Tilemap only supports orthogonal maps");
        }

        this->grid = j["grid"].get<tilemap_grid>();
        this->sheets = j["tilesets"].get<std::vector<sheet_info>>();
        if (j.count("object_types"))
        {
            this->obj_data = j["object_types"].get<std::vector<extended_object_data>>();
        }

        for (auto& j_layer : j["layers"])
        {
            this->layers.emplace_back(j_layer, this);

            if (j_layer.count("tileset_index"))
            {
                auto tileset_index = j_layer["tileset_index"].get<int32>();
                const auto& sheet = this->sheets.at(tileset_index);
                if (sheet.type == asset_pack::asset_type_tileset)
                {
                    auto& layer = this->layers.back();
                    if (RDGE_UNLIKELY(layer.type != LayerType::TILELAYER))
                    {
                        std::ostringstream ss;
                        ss << "Tilemap Layer[" << layer.name << "] type mismatch."
                           << " asset.type=" << sheet.type
                           << " layer.type=" << layer.type;

                        throw std::invalid_argument(ss.str());
                    }

                    layer.tilelayer.tileset = packfile.GetAsset<Tileset>(sheet.table_id);
                }
                else if (sheet.type == asset_pack::asset_type_spritesheet)
                {
                    auto& layer = this->layers.back();
                    if (RDGE_UNLIKELY(layer.type != LayerType::OBJECTGROUP))
                    {
                        std::ostringstream ss;
                        ss << "Tilemap Layer[" << layer.name << "] type mismatch."
                           << " asset.type=" << sheet.type
                           << " layer.type=" << layer.type;

                        throw std::invalid_argument(ss.str());
                    }

                    layer.objectgroup.spritesheet = packfile.GetAsset<SpriteSheet>(sheet.table_id);
                }
            }
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
        if (RDGE_UNLIKELY(layer.type != LayerType::TILELAYER || !layer.tilelayer.tileset))
        {
            std::ostringstream ss;
            ss << "Cannot create TileLayer:"
               << " name=" << layer.name
               << " type=" << layer.type
               << " tileset=" << static_cast<void*>(layer.tilelayer.tileset.get());
            throw std::invalid_argument(ss.str());
        }

        return TileLayer(layer, scale);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

SpriteLayer
Tilemap::CreateSpriteLayer (int32 layer_id, float scale)
{
    try
    {
        const auto& layer = this->layers.at(layer_id);
        if (RDGE_UNLIKELY(layer.type != LayerType::OBJECTGROUP ||
                          !layer.objectgroup.spritesheet))
        {
            std::ostringstream ss;
            ss << "Cannot create SpriteLayer:"
               << " name=" << layer.name
               << " type=" << layer.type
               << " spritesheet=" << static_cast<void*>(layer.objectgroup.spritesheet.get());
            throw std::invalid_argument(ss.str());
        }

        return SpriteLayer(layer, scale);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

const extended_object_data*
Tilemap::GetSharedObjectData (const std::string& key) const
{
    for (const auto& data : this->obj_data)
    {
        if (data.name == key)
        {
            return &data;
        }
    }

    return nullptr;
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
try_parse (const std::string& test, tilemap::Orientation& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "orthogonal") { out = tilemap::Orientation::ORTHOGONAL; return true; }
    else if (s == "isometric")  { out = tilemap::Orientation::ISOMETRIC;  return true; }
    else if (s == "staggered")  { out = tilemap::Orientation::STAGGERED;  return true; }
    else if (s == "hexagonal")  { out = tilemap::Orientation::HEXAGONAL;  return true; }

    return false;
}

} // namespace rdge
