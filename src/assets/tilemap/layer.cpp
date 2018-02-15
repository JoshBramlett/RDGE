#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>
#include <cstring> // strrchr

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

void
from_json (const nlohmann::json& j, Layer::tilelayer_data& layer)
{
    JSON_VALIDATE_REQUIRED(j, startx, is_number);
    JSON_VALIDATE_REQUIRED(j, starty, is_number);
    JSON_VALIDATE_REQUIRED(j, width, is_number);
    JSON_VALIDATE_REQUIRED(j, height, is_number);
    JSON_VALIDATE_OPTIONAL(j, data, is_array);
    JSON_VALIDATE_OPTIONAL(j, chunks, is_array);

    // Overwrite pos/size of the global grid to be local to the layer
    layer.grid.pos.x = j["startx"].get<decltype(layer.grid.pos.x)>();
    layer.grid.pos.y = j["starty"].get<decltype(layer.grid.pos.y)>();
    layer.grid.size.w = j["width"].get<decltype(layer.grid.size.w)>();
    layer.grid.size.h = j["height"].get<decltype(layer.grid.size.h)>();

    // fixed size map
    if (j.count("data"))
    {
        std::vector<Layer::tilelayer_data::tile_chunk>(1).swap(layer.chunks);

        auto& chunk = layer.chunks.at(0);
        chunk.coord.x = 0;
        chunk.coord.y = 0;
        chunk.data = j["data"].get<decltype(chunk.data)>();
    }
    // infinite size map
    else if (j.count("chunks"))
    {
        const auto& j_chunks = j["chunks"];
        std::vector<Layer::tilelayer_data::tile_chunk>(j_chunks.size()).swap(layer.chunks);

        size_t index = 0;
        for (const auto& j_chunk : j_chunks)
        {
            JSON_VALIDATE_REQUIRED(j_chunk, x, is_number);
            JSON_VALIDATE_REQUIRED(j_chunk, y, is_number);
            JSON_VALIDATE_REQUIRED(j_chunk, data, is_array);

            auto& chunk = layer.chunks.at(index++);
            chunk.coord.x = j_chunk["x"].get<decltype(chunk.coord.x)>();
            chunk.coord.y = j_chunk["y"].get<decltype(chunk.coord.y)>();
            chunk.data = j_chunk["data"].get<decltype(chunk.data)>();
        }
    }
    else
    {
        throw std::invalid_argument("Tile layer has no tile data");
    }
}

void
from_json (const nlohmann::json& j, Layer::objectgroup_data& group)
{
    JSON_VALIDATE_REQUIRED(j, objects, is_array);
    JSON_VALIDATE_REQUIRED(j, draworder, is_string);

    if (!try_parse(j["draworder"].get<std::string>(), group.draw_order))
    {
        throw std::invalid_argument("Invalid SpriteRenderOrder");
    }

    for (const auto& j_obj : j["objects"])
    {
        group.objects.emplace_back(j_obj);
    }
}

Layer::Layer (Tilemap* parent, const nlohmann::json& j)
    : m_parent(parent)
{
    try
    {
        JSON_VALIDATE_REQUIRED(j, name, is_string);
        JSON_VALIDATE_REQUIRED(j, type, is_string);
        JSON_VALIDATE_REQUIRED(j, opacity, is_number);
        JSON_VALIDATE_REQUIRED(j, visible, is_boolean);

        JSON_VALIDATE_OPTIONAL(j, offsetx, is_number);
        JSON_VALIDATE_OPTIONAL(j, offsety, is_number);

        // required
        this->name = j["name"].get<decltype(this->name)>();
        this->opacity = j["opacity"].get<decltype(this->opacity)>();
        this->visible = j["visible"].get<decltype(this->visible)>();

        // optional
        this->offset.x = j.count("offsetx") ? j["offsetx"].get<float>() : 0.f;
        this->offset.y = j.count("offsety") ? j["offsety"].get<float>() : 0.f;
        this->properties = PropertyCollection(j);

        // type specific
        if (!try_parse(j["type"].get<std::string>(), this->type))
        {
            std::ostringstream ss;
            ss << "Unable to parse LayerType."
               << " value=" << j["obj_type"].get<std::string>();
            throw std::invalid_argument(ss.str());
        }

        switch (this->type)
        {
        case LayerType::TILELAYER:
            this->tilelayer = j.get<decltype(this->tilelayer)>();

            // populate remaining global fields from the parent
            if (m_parent)
            {
                this->tilelayer.grid.render_order = m_parent->grid.render_order;
                this->tilelayer.grid.cell_size = m_parent->grid.cell_size;
                this->tilelayer.grid.chunk_size = m_parent->grid.chunk_size;
            }
            break;
        case LayerType::OBJECTGROUP:
            this->objectgroup = j.get<decltype(this->objectgroup)>();
            break;
        case LayerType::IMAGELAYER:
            //throw std::invalid_argument("LayerType::IMAGELAYER currently unsupported");
            break;
        case LayerType::GROUP:
            // TODO In order for this to work, I'd need to move assigning the
            //      spritesheet/tileset from the tilemap to the ctor.  In order
            //      for this to work I'd also need to pass the pack file in.
            throw std::invalid_argument("LayerType::GROUP currently unsupported");
        case LayerType::INVALID:
        default:
            throw std::invalid_argument("Invalid LayerType");
        }
    }
    catch (const std::exception& ex)
    {
        std::ostringstream ss;
        ss << "Tilemap::Layer[" << this->name << "]: " << ex.what();
        RDGE_THROW(ss.str());
    }
}

std::ostream&
operator<< (std::ostream& os, LayerType value)
{
    return os << rdge::to_string(value);
}

} // namespace tilemap

std::string
to_string (tilemap::LayerType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(tilemap::LayerType::INVALID)
        CASE(tilemap::LayerType::TILELAYER)
        CASE(tilemap::LayerType::OBJECTGROUP)
        CASE(tilemap::LayerType::IMAGELAYER)
        CASE(tilemap::LayerType::GROUP)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, tilemap::LayerType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "tilelayer")   { out = tilemap::LayerType::TILELAYER;   return true; }
    else if (s == "objectgroup") { out = tilemap::LayerType::OBJECTGROUP; return true; }
    else if (s == "imagelayer")  { out = tilemap::LayerType::IMAGELAYER;  return true; }
    else if (s == "group")       { out = tilemap::LayerType::GROUP;       return true; }

    return false;
}

} // namespace rdge
