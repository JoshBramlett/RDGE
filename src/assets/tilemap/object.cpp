#include <rdge/assets/tilemap/object.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>
#include <cstring>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

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
        this->id = j["id"].get<int32>();
        this->name = j["name"].get<std::string>();
        this->custom_type = j["type"].get<std::string>();
        this->visible = j["visible"].get<bool>();

        auto x = j["x"].get<int32>();
        auto y = j["y"].get<int32>();
        this->position = math::vec2(static_cast<float>(x), static_cast<float>(y));

        // optional
        this->properties = PropertyCollection(j);

        // type specific
        if (!try_parse(j["obj_type"].get<std::string>(), this->type))
        {
            std::ostringstream ss;
            ss << "Object[" << this->id << "] unable to parse object type."
               << " obj_type=" << j["obj_type"].get<std::string>();
            throw std::invalid_argument(ss.str());
        }

        switch (this->type)
        {
        case ObjectType::SPRITE:
            JSON_VALIDATE_REQUIRED(j, gid, is_number);
            JSON_VALIDATE_REQUIRED(j, width, is_number);
            JSON_VALIDATE_REQUIRED(j, height, is_number);
            JSON_VALIDATE_REQUIRED(j, rotation, is_number);

            m_gid = j["gid"].get<int32>();
            m_size = math::vec2(j["width"].get<float>(), j["height"].get<float>());
            m_rotation = j["rotation"].get<float>();

            if (m_gid <= 0)
            {
                std::ostringstream ss;
                ss << "Object[" << this->id << "] has an invalid gid";
                throw std::invalid_argument(ss.str());
            }
            break;
        case ObjectType::POINT:
            break;
        case ObjectType::CIRCLE:
            JSON_VALIDATE_REQUIRED(j, radius, is_number_float);

            m_radius = j["radius"].get<float>();
            break;
        case ObjectType::POLYGON:
            JSON_VALIDATE_REQUIRED(j, coords, is_array);
            m_numVerts = j["coords"].size();
            if (m_numVerts == 0)
            {
                std::ostringstream ss;
                ss << "Object[" << this->id << "] polygon has no vertices";
                throw std::invalid_argument(ss.str());
            }
            else if (m_numVerts > physics::polygon::MAX_VERTICES)
            {
                std::ostringstream ss;
                ss << "Object[" << this->id << "] polygon exceeds max vertex count";
                throw std::invalid_argument(ss.str());
            }

            for (size_t i = 0; i < m_numVerts; i++)
            {
                const auto& coord = j["coords"][i];
                JSON_VALIDATE_REQUIRED(coord, x, is_number);
                JSON_VALIDATE_REQUIRED(coord, y, is_number);

                m_vertices[i] = math::vec2(coord["x"].get<float>(), coord["y"].get<float>());
            }
            break;
        case ObjectType::POLYLINE:
            throw std::invalid_argument("ObjectType::POLYLINE currently unsupported");
        case ObjectType::TEXT:
            throw std::invalid_argument("ObjectType::TEXT currently unsupported");
        case ObjectType::INVALID:
        default:
            throw std::invalid_argument("Invalid ObjectType. name=" + this->name);
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

math::vec2
Object::GetPoint (void) const
{
    if (this->type != ObjectType::POINT)
    {
        std::ostringstream ss;
        ss << "Object[" << this->id << "] cannot build POINT "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return this->position;
}

physics::circle
Object::GetCircle (void) const
{
    if (this->type != ObjectType::CIRCLE)
    {
        std::ostringstream ss;
        ss << "Object[" << this->id << "] cannot build circle "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return physics::circle(this->position, m_size.x);
}

physics::polygon
Object::GetPolygon (void) const
{
    if (this->type != ObjectType::POLYGON)
    {
        std::ostringstream ss;
        ss << "Object[" << this->id << "] cannot build polygon "
           << "from an object of type " << this->type;
        throw std::invalid_argument(ss.str());
    }

    return physics::polygon(m_vertices, m_numVerts);
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
