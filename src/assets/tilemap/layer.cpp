#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

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
        JSON_VALIDATE_OPTIONAL(j, tileset_index, is_number);

        // required
        this->name = j["name"].get<decltype(this->name)>();
        if (!try_parse(j["type"].get<std::string>(), this->type))
        {
            throw std::invalid_argument("Invalid Layer type. name=" + this->name);
        }

        this->visible = j["visible"].get<decltype(this->visible)>();
        this->opacity = j["opacity"].get<decltype(this->opacity)>();

        // optional
        this->properties = PropertyCollection(j);
        this->offset = math::vec2(j.count("offsetx") ? j["offsetx"].get<float>() : 0.f,
                                  j.count("offsety") ? j["offsety"].get<float>() : 0.f);
        if (j.count("tileset_index"))
        {
            this->tileset_index = j["tileset_index"].get<decltype(this->tileset_index)>();
        }

        // type specific
        if (this->type == LayerType::TILELAYER)
        {
            JSON_VALIDATE_REQUIRED(j, startx, is_number);
            JSON_VALIDATE_REQUIRED(j, starty, is_number);
            JSON_VALIDATE_REQUIRED(j, width, is_number);
            JSON_VALIDATE_REQUIRED(j, height, is_number);
            JSON_VALIDATE_OPTIONAL(j, data, is_array);
            JSON_VALIDATE_OPTIONAL(j, chunks, is_array);

            this->grid_location.x = j["startx"].get<decltype(this->grid_location.x)>();
            this->grid_location.y = j["starty"].get<decltype(this->grid_location.y)>();
            this->grid_size.w = j["width"].get<decltype(this->grid_size.w)>();
            this->grid_size.h = j["height"].get<decltype(this->grid_size.h)>();

            // fixed size map
            if (j.count("data"))
            {
                std::vector<tile_chunk>(1).swap(this->chunks);

                auto& chunk = this->chunks.at(0);
                chunk.x = 0;
                chunk.y = 0;
                chunk.data = j["data"].get<decltype(chunk.data)>();
            }
            // infinite size map
            else if (j.count("chunks"))
            {
                const auto& j_chunks = j["chunks"];
                std::vector<tile_chunk>(j_chunks.size()).swap(this->chunks);

                size_t index = 0;
                for (const auto& j_chunk : j_chunks)
                {
                    JSON_VALIDATE_REQUIRED(j_chunk, x, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, y, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, data, is_array);

                    auto& chunk = this->chunks.at(index++);
                    chunk.x = j_chunk["x"].get<decltype(chunk.x)>();
                    chunk.y = j_chunk["y"].get<decltype(chunk.y)>();
                    chunk.data = j_chunk["data"].get<decltype(chunk.data)>();
                }
            }
            else
            {
                throw std::invalid_argument("Layer cannot find data. name=" + this->name);
            }
        }
        else if (this->type == LayerType::OBJECTGROUP)
        {
            JSON_VALIDATE_REQUIRED(j, objects, is_array);
            JSON_VALIDATE_REQUIRED(j, draworder, is_string);

            if (!try_parse(j["draworder"].get<std::string>(), this->draw_order))
            {
                throw std::invalid_argument("Invalid SpriteRenderOrder. name=" + this->name);
            }

            for (const auto& j_obj : j["objects"])
            {
                this->objects.emplace_back(j_obj);
            }
        }
        else if (this->type == LayerType::IMAGELAYER)
        {
            // TODO
            // this->image = path;
        }
        else if (this->type == LayerType::GROUP)
        {
            for (auto& l : j["layers"])
            {
                this->layers.emplace_back(m_parent, l);
            }
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
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
