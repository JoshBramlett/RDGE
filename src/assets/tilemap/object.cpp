#include <rdge/assets/tilemap/object.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>
#include <cstring>

namespace rdge {
namespace tilemap {

using json = nlohmann::json;

namespace {

void
VerifyAndSetType (Object& o, ObjectType t)
{
    if (o.type != ObjectType::AABB)
    {
        std::ostringstream ss;
        ss << "Object[" << o.id << "] deduced conflicting types. "
           << o.type << "and " << t;

        throw std::invalid_argument(ss.str());
    }

    o.type = t;
}

} // anonymous namespace

Object::Object (const nlohmann::json& j)
{
    // Tiled format
    // http://doc.mapeditor.org/en/latest/reference/json-map-format/

    try
    {
        JSON_VALIDATE_REQUIRED(j, id, is_number);
        JSON_VALIDATE_REQUIRED(j, name, is_string);
        JSON_VALIDATE_REQUIRED(j, type, is_string);
        JSON_VALIDATE_REQUIRED(j, x, is_number);
        JSON_VALIDATE_REQUIRED(j, y, is_number);
        JSON_VALIDATE_REQUIRED(j, width, is_number);
        JSON_VALIDATE_REQUIRED(j, height, is_number);
        JSON_VALIDATE_REQUIRED(j, visible, is_boolean);
        JSON_VALIDATE_REQUIRED(j, rotation, is_number);

        JSON_VALIDATE_OPTIONAL(j, gid, is_number);
        JSON_VALIDATE_OPTIONAL(j, point, is_boolean);
        JSON_VALIDATE_OPTIONAL(j, ellipse, is_boolean);
        JSON_VALIDATE_OPTIONAL(j, polygon, is_array);
        JSON_VALIDATE_OPTIONAL(j, polyline, is_array);
        JSON_VALIDATE_OPTIONAL(j, text, is_string);

        // required
        this->id = j["id"].get<int32>();
        this->name = j["name"].get<std::string>();
        this->custom_type = j["type"].get<std::string>();
        this->visible = j["visible"].get<bool>();
        this->rotation = j["rotation"].get<float>();

        auto x = j["x"].get<int32>();
        auto y = j["y"].get<int32>();
        this->position = math::vec2(static_cast<float>(x), static_cast<float>(y));

        auto w = j["width"].get<int32>();
        auto h = j["height"].get<int32>();
        m_size = math::vec2(static_cast<float>(w), static_cast<float>(h));

        // optional
        this->properties = PropertyCollection(j);

        // type deduction
        this->type = ObjectType::AABB;

        if (j.count("gid"))
        {
            this->type = ObjectType::SPRITE;

            m_gid = j["gid"].get<int32>();
            if (m_gid <= 0)
            {
                std::ostringstream ss;
                ss << "Object[" << this->id << "] has an invalid gid";

                throw std::invalid_argument(ss.str());
            }
        }

        if (j.count("point") && j["point"].get<bool>())
        {
            VerifyAndSetType(*this, ObjectType::POINT);
        }

        if (j.count("ellipse") && j["ellipse"].get<bool>())
        {
            VerifyAndSetType(*this, ObjectType::CIRCLE);

            if (w != h)
            {
                std::ostringstream ss;
                ss << "Object[" << this->id << "] only supports circles";

                throw std::invalid_argument(ss.str());
            }
        }

        if (j.count("polygon"))
        {
            VerifyAndSetType(*this, ObjectType::POLYGON);

            const auto& coords = j["polygon"];
            m_numVerts = coords.size();
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
                const auto& coord = coords[i];
                JSON_VALIDATE_REQUIRED(coord, x, is_number);
                JSON_VALIDATE_REQUIRED(coord, y, is_number);

                m_vertices[i] = math::vec2(coord["x"].get<float>(), coord["y"].get<float>());
            }
        }

        if (j.count("polyline"))
        {
            // TODO
            //VerifyAndSetType(*this, ObjectType::POLYLINE);

            std::ostringstream ss;
            ss << "Object[" << this->id << "] polyline not supported";

            throw std::invalid_argument(ss.str());
        }

        if (j.count("text"))
        {
            // TODO
            //VerifyAndSetType(*this, ObjectType::TEXT);

            std::ostringstream ss;
            ss << "Object[" << this->id << "] text not supported";

            throw std::invalid_argument(ss.str());
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

physics::aabb
Object::GetAABB (void) const
{
    if (this->type != ObjectType::AABB)
    {
        std::ostringstream ss;
        ss << "Object[" << this->id << "] cannot build AABB "
           << "from an object of type " << this->type;

        throw std::invalid_argument(ss.str());
    }

    float half_x = m_size.x * 0.5f;
    float half_y = m_size.y * 0.5f;
    return physics::aabb(math::vec2(this->position.x - half_x, this->position.y - half_y),
                         math::vec2(this->position.x + half_x, this->position.y + half_y));
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
    if (this->type == ObjectType::AABB)
    {
        return physics::polygon(m_size.w * 0.5f,
                                m_size.h * 0.5f,
                                this->position,
                                math::to_radians(this->rotation));
    }
    else if (this->type == ObjectType::POLYGON)
    {
        physics::polygon result(m_vertices, m_numVerts);
        result.rotate(math::to_radians(this->rotation));

        return result;
    }

    std::ostringstream ss;
    ss << "Object[" << this->id << "] cannot build polygon "
       << "from an object of type " << this->type;

    throw std::invalid_argument(ss.str());
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
        CASE(tilemap::ObjectType::AABB)
        CASE(tilemap::ObjectType::POLYGON)
        CASE(tilemap::ObjectType::POLYLINE)
        CASE(tilemap::ObjectType::TEXT)
        default: break;
#undef CASE
    }

    return "UNKNOWN";
}

} // namespace rdge
