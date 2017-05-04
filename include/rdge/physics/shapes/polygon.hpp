//! \headerfile <rdge/physics/shapes/polygon.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/collision.hpp>

#include <array>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \struct polygon
//! \brief Convex polygon whose vertices are in CCW order
//! \details Structure has a maximum number of vertices defined by \ref
//!          MAX_VERTICES.
struct polygon : public ishape
{
    static constexpr uint8 MAX_VERTICES = 8; //!< Maximum number of vertices

    using PolygonData = std::array<math::vec2, MAX_VERTICES>; //!< Vertex container

    // TODO vec2 centroid;        //!< Position at center
    PolygonData vertices; //!< Collection of vertices
    PolygonData normals;  //!< Edge normals
    size_t count = 0;     //!< Number of vertices

    //! \brief polygon default ctor
    //! \details Zero initialization
    constexpr polygon (void)
        //: centroid(0.f, 0.f)
    {
        vertices.fill(0.f);
        normals.fill(0.f);
    }

    //! \brief polygon ctor
    //! \details Initialize polygon from a list of vertices
    //! \param [in] verts List of vertices
    //! \param [in] count Number of vertices in the array
    polygon (const PolygonData& verts, size_t count);

    //! \returns Underlying type (used for casting)
    virtual ShapeType type (void) const noexcept override
    {
        return ShapeType::POLYGON;
    }

    //! \brief Check if a point resides within the polygon (edge exclusive)
    //! \param [in] point Point coordinates
    //! \returns True iff point is within the polygon
    //constexpr bool contains (const vec2& point) const noexcept
    //{
        //return (point - pos).self_dot() < square(radius);
    //}

    //! \brief Check if the polygon intersects with another (edge exclusive)
    //! \param [in] other polygon structure
    //! \returns True iff intersecting
    //constexpr bool intersects_with (const polygon& other) const noexcept
    //{
        //return (other.pos - pos).self_dot() < square(radius + other.radius);
    //}

    //! \brief Check if the polygon intersects with another (edge exclusive)
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other polygon structure
    //! \param [out] mf Manifold containing resolution
    //! \returns True iff intersecting
    //bool intersects_with (const polygon& other, physics::collision_manifold& mf) const noexcept;

    //! \brief Create a rectangle polygon
    //! \param [in] he_x x-axis half extent
    //! \param [in] he_y y-axis half extent
    //! \returns Rectangle polygon
    static polygon create_box (float he_x, float he_y)
    {
        polygon result;

        result.count = 4;
        result.vertices[0] = { -he_x, -he_y };
        result.vertices[1] = {  he_x, -he_y };
        result.vertices[2] = {  he_x,  he_y };
        result.vertices[3] = { -he_x,  he_y };
        result.normals[0] = { 0.f, -1.f };
        result.normals[1] = { 1.f, 0.f };
        result.normals[2] = { 0.f, 1.f };
        result.normals[3] = { -1.f, 0.f };

        return result;
    }
};

//! \brief polygon equality operator
//! \returns True iff identical
inline bool operator== (const polygon& lhs, const polygon& rhs) noexcept
{
    if (lhs.count == rhs.count)
    {
        for (size_t i = 0; i < lhs.count; ++i)
        {
            if (lhs.vertices[i] != rhs.vertices[i])
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

//! \brief polygon inequality operator
//! \returns True iff not identical
inline bool operator!= (const polygon& lhs, const polygon& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief aabb stream output operator
//inline std::ostream& operator<< (std::ostream& os, const polygon& c)
//{
    //return os << "[ " << c.pos << ", r=" << c.radius << " ]";
//}

} // namespace physics
} // namespace physics
