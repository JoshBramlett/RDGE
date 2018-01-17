#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

Layer::Layer (const nlohmann::json& j)
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
        this->name = j["name"].get<std::string>();
        if (!try_parse(j["type"].get<std::string>(), this->type))
        {
            throw std::invalid_argument("Layer invalid type. name=" + this->name);
        }

        this->visible = j["visible"].get<bool>();
        this->opacity = j["opacity"].get<float>();

        // optional
        this->properties = PropertyCollection(j);
        this->offset = math::vec2(j.count("offsetx") ? j["offsetx"].get<float>() : 0.f,
                                  j.count("offsety") ? j["offsety"].get<float>() : 0.f);
        if (j.count("tileset_index"))
        {
            this->tileset_index = j["tileset_index"].get<int32>();
        }

        // type specific
        if (this->type == LayerType::TILELAYER)
        {
            JSON_VALIDATE_OPTIONAL(j, data, is_array);
            JSON_VALIDATE_OPTIONAL(j, chunks, is_array);

            // fixed size map
            if (j.count("data"))
            {
                this->chunks = std::vector<tile_chunk>(1);

                auto& chunk = this->chunks[0];
                chunk.x = 0;
                chunk.y = 0;
                chunk.data = j["data"].get<std::vector<uint32>>();
            }
            // infinite size map
            else if (j.count("chunks"))
            {
                const auto& j_chunks = j["chunks"];
                this->chunks = std::vector<tile_chunk>(j_chunks.size());

                size_t index = 0;
                for (const auto& j_chunk : j_chunks)
                {
                    JSON_VALIDATE_REQUIRED(j_chunk, x, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, y, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, data, is_array);

                    auto& chunk = this->chunks[index++];
                    chunk.x = j_chunk["x"].get<size_t>();
                    chunk.y = j_chunk["y"].get<size_t>();
                    chunk.data = j_chunk["data"].get<std::vector<uint32>>();
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
                throw std::invalid_argument("Layer invalid draworder. name=" + this->name);
            }

            for (const auto& j_obj : j["objects"])
            {
                this->objects.emplace_back(Object(j_obj));
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
                this->layers.emplace_back(Layer(l));
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
