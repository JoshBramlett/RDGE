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
        JSON_VALIDATE_REQUIRED(j, opacity, is_number_float);
        JSON_VALIDATE_REQUIRED(j, visible, is_boolean);

        JSON_VALIDATE_OPTIONAL(j, offsetx, is_number);
        JSON_VALIDATE_OPTIONAL(j, offsety, is_number);
        JSON_VALIDATE_OPTIONAL(j, tileset_id, is_number);

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
        if (j.count("tileset_id"))
        {
            this->tileset_index = j["tileset_id"].get<int32>();
        }

        // type specific
        if (this->type == LayerType::TILELAYER)
        {
            JSON_VALIDATE_REQUIRED(j, width, is_number);
            JSON_VALIDATE_REQUIRED(j, height, is_number);

            JSON_VALIDATE_OPTIONAL(j, data, is_array);
            JSON_VALIDATE_OPTIONAL(j, chunks, is_array);

            this->rows = j["width"].get<size_t>();
            this->cols = j["height"].get<size_t>();

            if (j.count("data"))
            {
                // fixed size map
                this->data = j["data"].get<std::vector<uint32>>();
            }
            else if (j.count("chunks"))
            {
                // infinite size map
                JSON_VALIDATE_REQUIRED(j, startx, is_number);
                JSON_VALIDATE_REQUIRED(j, starty, is_number);

                this->data.assign(this->rows * this->cols, 0);
                this->start_x = j["startx"].get<size_t>();
                this->start_y = j["starty"].get<size_t>();

                for (const auto& j_chunk : j["chunks"])
                {
                    JSON_VALIDATE_REQUIRED(j_chunk, x, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, y, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, width, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, height, is_number);
                    JSON_VALIDATE_REQUIRED(j_chunk, data, is_array);

                    size_t x = j_chunk["x"].get<size_t>() - this->start_x;
                    size_t y = j_chunk["y"].get<size_t>() - this->start_y;
                    size_t w = j_chunk["width"].get<size_t>();
                    size_t h = j_chunk["height"].get<size_t>();

                    const auto& chunk_data = j_chunk["data"];
                    for (size_t row = 0; row < h, i++)
                    {
                        for (size_t col = 0; col < w, i++)
                        {
                            size_t data_idx = ((y + row) * this->rows) + (x + col);
                            size_t chunk_idx = (row * h) + col;
                            this->data[data_idx] = chunk_data[chunk_idx].get<uint32>();
                        }
                    }
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

TileLayer
Layer::GenerateTileLayer (void) const
{

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
