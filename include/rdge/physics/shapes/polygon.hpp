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
    //! \var Maximum number of vertices
    static constexpr uint8 MAX_VERTICES = 8;

    //! \var Padding between the polygon vertices and it's aabb edges.  Has a
    //!      distinct relationship with the linear slop as it provides a sufficient
    //!      buffer for continuous collision.
    static constexpr float AABB_PADDING = LINEAR_SLOP * 2.f;

    //! \typedef Vertex container
    using PolygonData = std::array<math::vec2, MAX_VERTICES>;

    math::vec2 centroid;  //!< Position at center
    PolygonData vertices; //!< Collection of vertices
    PolygonData normals;  //!< Edge normals
    size_t count = 0;     //!< Number of vertices

    //! \brief polygon default ctor
    //! \details Zero initialization.
    constexpr polygon (void)
    {
        vertices.fill(0.f);
        normals.fill(0.f);
    }

    //! \brief polygon ctor
    //! \details Initialize polygon from a list of vertices.
    //! \param [in] verts List of vertices
    //! \param [in] num_verts Number of vertices in the array
    explicit polygon (const PolygonData& verts, size_t num_verts);

    //! \brief polygon generic rectangle specialization ctor
    //! \details Initializes a box around the origin extending the length of the
    //!          provided half extents.
    //! \param [in] he_x x-axis half extent
    //! \param [in] he_y y-axis half extent
    explicit polygon (float he_x, float he_y);

    //!@{
    //! \brief polygon explicit vertices ctor
    //! \details Initialize a polygon from the list of vertices
    explicit polygon (const math::vec2& p1, const math::vec2& p2, const math::vec2& p3);
    explicit polygon (const math::vec2& p1, const math::vec2& p2,
                      const math::vec2& p3, const math::vec2& p4);
    explicit polygon (const math::vec2& p1, const math::vec2& p2, const math::vec2& p3,
                      const math::vec2& p4, const math::vec2& p5);
    //!@}

    //! \returns Underlying type
    ShapeType type (void) const noexcept override { return ShapeType::POLYGON; }

    bool contains (const iso_transform& xf, const math::vec2& p) const override
    {
        math::vec2 p_local = xf.rot.inv_rotate(p - xf.pos);
        for (size_t i = 0; i < count; ++i)
        {
            // dot product with a unit vector provides the vector lengths on the axes
            // of the unit vector.  Because the vertices are in CCW order, the normals
            // should be pointing to their left, so the value should be negative.
            if (math::dot(normals[i], p_local - vertices[i]) > 0.f)
            {
                return false;
            }
        }

        return true;
    }

    aabb compute_aabb (const iso_transform& xf) const override
    {
        math::vec2 lo = xf.rot.rotate(vertices[0]);
        math::vec2 hi = lo;

        for (size_t i = 0; i < count; ++i)
        {
            math::vec2 v = xf.rot.rotate(vertices[i]);
            lo.x = std::min(lo.x, v.x);
            lo.y = std::min(lo.y, v.y);
            hi.x = std::max(hi.x, v.x);
            hi.y = std::max(hi.y, v.y);
        }

        return aabb(lo - AABB_PADDING, hi + AABB_PADDING);
    }

    //! \brief Compute the mass and analog data
    //! \param [in] density Density of the shape
    //! \returns Mass, centroid, and mass moment of inertia
    //! \see https://en.wikipedia.org/wiki/Centroid#Centroid_of_a_polygon
    //! \see https://en.wikipedia.org/wiki/List_of_moments_of_inertia
    mass_data compute_mass (float density) const override;

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
