#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

Layer::Layer (const nlohmann::json& j)
{
    // Tiled format

    // tilelayer
    //{
      //"data":[1, 2, 1, 2, 3, 1, 3, 1, 2, 2, 3, 3, 4, 4, 4, 1],
      //"height":4,
      //"name":"ground",
      //"opacity":1,
      //"type":"tilelayer",
      //"visible":true,
      //"width":4,
      //"x":0,
      //"y":0
    //}

    // objectgroup
    //{
      //"draworder":"topdown",
      //"height":0,
      //"name":"people",
      //"objects":[ ],
      //"opacity":1,
      //"type":"objectgroup",
      //"visible":true,
      //"width":0,
      //"x":0,
      //"y":0
    //}

    try
    {
        JSON_VALIDATE_REQUIRED(j, name, is_string);
        JSON_VALIDATE_REQUIRED(j, type, is_string);
        JSON_VALIDATE_REQUIRED(j, x, is_number);
        JSON_VALIDATE_REQUIRED(j, y, is_number);
        JSON_VALIDATE_REQUIRED(j, width, is_number);
        JSON_VALIDATE_REQUIRED(j, height, is_number);
        JSON_VALIDATE_REQUIRED(j, visible, is_boolean);
        JSON_VALIDATE_REQUIRED(j, opacity, is_number);

        JSON_VALIDATE_OPTIONAL(j, offsetx, is_number);
        JSON_VALIDATE_OPTIONAL(j, offsety, is_number);
        JSON_VALIDATE_OPTIONAL(j, data, is_array);
        JSON_VALIDATE_OPTIONAL(j, objects, is_array);
        JSON_VALIDATE_OPTIONAL(j, draworder, is_string);

        // required
        this->name = j["name"].get<std::string>();
        if (!try_parse(j["type"].get<std::string>(), this->type))
        {
            throw std::invalid_argument("Layer invalid type. name=" + this->name);
        }

        this->visible = j["visible"].get<bool>();
        this->opacity = j["opacity"].get<float>();
        this->rows = j["width"].get<int32>();
        this->cols = j["height"].get<int32>();

        // unused (tiled docs say values are always set to zero)
        //auto x = j["x"].get<int32>();
        //auto y = j["y"].get<int32>();

        // optional
        this->properties = PropertyCollection(j);
        this->offset = math::vec2(j.count("offsetx") ? j["offsetx"].get<float>() : 0.f,
                                  j.count("offsety") ? j["offsety"].get<float>() : 0.f);

        // type specific
        if (this->type == LayerType::TILE)
        {
            this->data = j["data"].get<std::vector<int32>>();
        }
        else if (this->type == LayerType::OBJECT)
        {
            auto d = j["draworder"].get<std::string>();
            if (d == "topdown")
            {
                this->draw_order = object_draw_topdown;
            }
            else if (d == "index")
            {
                this->draw_order = object_draw_index;
            }
            else
            {
                throw std::invalid_argument("Layer invalid draworder. name=" + this->name);
            }

            for (auto& o : j["objects"])
            {
                this->objects.emplace_back(Object(o));
            }
        }
        else if (this->type == LayerType::IMAGE)
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
        CASE(tilemap::LayerType::TILE)
        CASE(tilemap::LayerType::OBJECT)
        CASE(tilemap::LayerType::IMAGE)
        CASE(tilemap::LayerType::GROUP)
        default: break;
#undef CASE
    }

    return "UNKNOWN";
}

bool
try_parse (const std::string& s, tilemap::LayerType& out)
{
    if      (s == "tilelayer")   { out = tilemap::LayerType::TILE;   return true; }
    else if (s == "objectgroup") { out = tilemap::LayerType::OBJECT; return true; }
    else if (s == "imagelayer")  { out = tilemap::LayerType::IMAGE;  return true; }
    else if (s == "group")       { out = tilemap::LayerType::GROUP;  return true; }

    return false;
}

} // namespace rdge
