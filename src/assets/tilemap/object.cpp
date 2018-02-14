#include <rdge/assets/tilemap/object.hpp>
#include <rdge/physics/shapes/circle.hpp>
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
from_json (const nlohmann::json& j, Object::object_sprite_data& sprite)
{
    JSON_VALIDATE_REQUIRED(j, gid, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, width, is_number);
    JSON_VALIDATE_REQUIRED(j, height, is_number);
    JSON_VALIDATE_REQUIRED(j, rotation, is_number);

    sprite.gid = j["gid"].get<decltype(sprite.gid)>();
    sprite.size.w = j["width"].get<decltype(sprite.size.w)>();
    sprite.size.h = j["height"].get<decltype(sprite.size.h)>();
    sprite.rotation = j["rotation"].get<decltype(sprite.rotation)>();

    if (RDGE_UNLIKELY(sprite.gid == 0))
    {
        throw std::invalid_argument("Sprite must have a valid GID");
    }
}

void
from_json (const nlohmann::json& j, Object::object_circle_data& circle)
{
    JSON_VALIDATE_REQUIRED(j, radius, is_number);

    circle.radius = j["radius"].get<decltype(circle.radius)>();
}

void
from_json (const nlohmann::json& j, Object::object_polygon_data& poly)
{
    JSON_VALIDATE_REQUIRED(j, coords, is_array);
    poly.vertex_count = j["coords"].size();

    if (RDGE_UNLIKELY(poly.vertex_count == 0))
    {
        throw std::invalid_argument("Polygon has no vertex data");
    }
    else if (RDGE_UNLIKELY(poly.vertex_count > physics::polygon::MAX_VERTICES))
    {
        throw std::invalid_argument("Polygon vertex count exceeds max");
    }
    else
    {
        for (size_t i = 0; i < poly.vertex_count; i++)
        {
            const auto& j_coord = j["coords"][i];
            JSON_VALIDATE_REQUIRED(j_coord, x, is_number);
            JSON_VALIDATE_REQUIRED(j_coord, y, is_number);

            poly.vertices[i].x = j_coord["x"].get<float>();
            poly.vertices[i].y = j_coord["y"].get<float>();
        }
    }
}

Object::Object (const nlohmann::json& j)
{
    try
    {
        JSON_VALIDATE_REQUIRED(j, obj_type, is_string);
        JSON_VALIDATE_REQUIRED(j, id, is_number);
        JSON_VALIDATE_REQUIRED(j, name, is_string);
        JSON_VALIDATE_REQUIRED(j, type, is_string);
        JSON_VALIDATE_REQUIRED(j, x, is_number);
        JSON_VALIDATE_REQUIRED(j, y, is_number);
        JSON_VALIDATE_REQUIRED(j, visible, is_boolean);

        // required
        this->id = j["id"].get<decltype(this->id)>();
        this->name = j["name"].get<decltype(this->name)>();
        this->custom_type = j["type"].get<decltype(this->custom_type)>();
        this->pos.x = j["x"].get<decltype(this->pos.x)>();
        this->pos.y = j["y"].get<decltype(this->pos.y)>();
        this->visible = j["visible"].get<decltype(this->visible)>();

        // optional
        this->properties = PropertyCollection(j);

        // type specific
        if (!try_parse(j["obj_type"].get<std::string>(), this->type))
        {
            std::ostringstream ss;
            ss << "Unable to parse ObjectType."
               << " value=" << j["obj_type"].get<std::string>();
            throw std::invalid_argument(ss.str());
        }

        switch (this->type)
        {
        case ObjectType::SPRITE:
            this->sprite = j.get<decltype(this->sprite)>();
            break;
        case ObjectType::POINT:
            // intentionally empty
            break;
        case ObjectType::CIRCLE:
            this->circle = j.get<decltype(this->circle)>();
            break;
        case ObjectType::POLYGON:
            this->polygon = j.get<decltype(this->polygon)>();
            break;
        case ObjectType::POLYLINE:
            throw std::invalid_argument("ObjectType::POLYLINE currently unsupported");
        case ObjectType::TEXT:
            throw std::invalid_argument("ObjectType::TEXT currently unsupported");
        case ObjectType::INVALID:
        default:
            throw std::invalid_argument("Invalid ObjectType");
        }
    }
    catch (const std::exception& ex)
    {
        std::ostringstream ss;
        ss << "Tilemap::Object[" << this->id << "]: " << ex.what();
        RDGE_THROW(ss.str());
    }
}

math::vec2
Object::GetPoint (void) const
{
    if (this->type != ObjectType::POINT)
    {
        std::ostringstream ss;
        ss << "Tilemap::Object[" << this->id << "] cannot build POINT "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return this->pos;
}

physics::circle
Object::GetCircle (void) const
{
    if (this->type != ObjectType::CIRCLE)
    {
        std::ostringstream ss;
        ss << "Tilemap::Object[" << this->id << "] cannot build circle "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return physics::circle(this->pos, this->circle.radius);
}

physics::polygon
Object::GetPolygon (void) const
{
    if (this->type != ObjectType::POLYGON)
    {
        std::ostringstream ss;
        ss << "Tilemap::Object[" << this->id << "] cannot build polygon "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return physics::polygon(this->polygon.vertices, this->polygon.vertex_count);
}

std::ostream&
operator<< (std::ostream& os, ObjectType value)
{
    return os << rdge::to_string(value);
}

} // namespace tilemap

std::string
to_string (tilemap::ObjectType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(tilemap::ObjectType::INVALID)
        CASE(tilemap::ObjectType::SPRITE)
        CASE(tilemap::ObjectType::POINT)
        CASE(tilemap::ObjectType::CIRCLE)
        CASE(tilemap::ObjectType::POLYGON)
        CASE(tilemap::ObjectType::POLYLINE)
        CASE(tilemap::ObjectType::TEXT)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, tilemap::ObjectType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "sprite")   { out = tilemap::ObjectType::SPRITE;   return true; }
    else if (s == "point")    { out = tilemap::ObjectType::POINT;    return true; }
    else if (s == "circle")   { out = tilemap::ObjectType::CIRCLE;   return true; }
    else if (s == "polygon")  { out = tilemap::ObjectType::POLYGON;  return true; }
    else if (s == "polyline") { out = tilemap::ObjectType::POLYLINE; return true; }
    else if (s == "text")     { out = tilemap::ObjectType::TEXT;     return true; }

    return false;
}

} // namespace rdge
