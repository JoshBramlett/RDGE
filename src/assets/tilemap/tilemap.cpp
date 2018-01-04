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

    // map
    //{
      //"backgroundcolor":"#656667",
      //"height":4,
      //"layers":[ ],
      //"nextobjectid":1,
      //"orientation":"orthogonal",
      //"properties": { },
      //"renderorder":"right-down",
      //"tileheight":32,
      //"tilesets":[ ],
      //"tilewidth":32,
      //"version":1,
      //"tiledversion":"1.0.3",
      //"width":4
    //}

    try
    {
        JSON_VALIDATE_REQUIRED(j, orientation, is_string);
        JSON_VALIDATE_REQUIRED(j, width, is_number);
        JSON_VALIDATE_REQUIRED(j, height, is_number);
        JSON_VALIDATE_REQUIRED(j, tilewidth, is_number);
        JSON_VALIDATE_REQUIRED(j, tileheight, is_number);
        JSON_VALIDATE_REQUIRED(j, layers, is_array);
        JSON_VALIDATE_REQUIRED(j, tilesets, is_array);

        JSON_VALIDATE_OPTIONAL(j, backgroundcolor, is_string);
        JSON_VALIDATE_OPTIONAL(j, renderorder, is_string);

        // required
        if (!try_parse(j["orientation"].get<std::string>(), this->orientation))
        {
            throw std::invalid_argument("Tilemap invalid orientation");
        }

        if (this->orientation == Orientation::ORTHOGONAL)
        {
            if (!try_parse(j["renderorder"].get<std::string>(), this->render_order))
            {
                throw std::invalid_argument("Tilemap invalid render_order");
            }
        }

        if (this->orientation != Orientation::ORTHOGONAL ||
            this->render_order != RenderOrder::RIGHT_DOWN)
        {
            throw std::invalid_argument("Tilemap only supports orthogonal right-down");
        }

        this->rows = j["width"].get<int32>();
        this->cols = j["height"].get<int32>();
        this->cell_size = math::vec2(j["tilewidth"].get<float>(),
                                     j["tileheight"].get<float>());

        for (auto& l : j["layers"])
        {
            this->layers.emplace_back(Layer(l));
        }

        for (auto& t : j["tilesets"])
        {
            this->tilesets.emplace_back(Tileset(t));
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

std::ostream&
operator<< (std::ostream& os, Orientation value)
{
    return os << rdge::to_string(value);
}

std::ostream&
operator<< (std::ostream& os, RenderOrder value)
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

std::string
to_string (tilemap::RenderOrder value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(tilemap::RenderOrder::INVALID)
        CASE(tilemap::RenderOrder::RIGHT_DOWN)
        CASE(tilemap::RenderOrder::RIGHT_UP)
        CASE(tilemap::RenderOrder::LEFT_DOWN)
        CASE(tilemap::RenderOrder::LEFT_UP)
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

bool
try_parse (const std::string& s, tilemap::RenderOrder& out)
{
    if      (s == "right-down") { out = tilemap::RenderOrder::RIGHT_DOWN; return true; }
    else if (s == "right-up")   { out = tilemap::RenderOrder::RIGHT_UP;   return true; }
    else if (s == "left-down")  { out = tilemap::RenderOrder::LEFT_DOWN;  return true; }
    else if (s == "left-up")    { out = tilemap::RenderOrder::LEFT_UP;    return true; }

    return false;
}

} // namespace rdge
