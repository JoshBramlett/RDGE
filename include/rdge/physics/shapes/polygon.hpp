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

    //! \brief Check if a point resides within the polygon (edge exclusive)
    //! \param [in] point Local point coordinates
    //! \returns True iff point is within the polygon
    bool contains (const math::vec2& point) const override;

    //! \brief Compute an aabb surrounding the polygon
    //! \note aabb edges will be padded by \ref AABB_PADDING
    //! \warning Resultant value may still need to be converted to world space
    //! \returns Surrounding aabb
    aabb compute_aabb (void) const override;

    //! \brief Compute the mass and analog data
    //! \param [in] density Density of the shape
    //! \returns Mass, centroid, and mass moment of inertia
    //! \see https://en.wikipedia.org/wiki/Centroid#Centroid_of_a_polygon
    //! \see https://en.wikipedia.org/wiki/List_of_moments_of_inertia
    mass_data compute_mass (float density) const override;

    //!@{ GJK support functions
    //! \brief Provides the first point in the array
    math::vec2 first_point (void) const override
    {
        return vertices[0];
    }

    //! \brief Retrieves the farthest point along the provided direction
    //! \param [in] d Direction to find the farthest point
    math::vec2 farthest_point (const math::vec2& d) const override
    {
        size_t index = 0;
        float max_product = d.dot(vertices[index]);
        for (size_t i = 1; i < count; ++i)
        {
            float p = d.dot(vertices[i]);
            if (p > max_product)
            {
                max_product = p;
                index = i;
            }
        }

        return vertices[index];
    }
    //!@}

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
