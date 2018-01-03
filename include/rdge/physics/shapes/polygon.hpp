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
#include <limits>
#include <utility>

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

    // TODO doc
    static constexpr float RELATIVE_TOLERANCE = LINEAR_SLOP * 0.1f;

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

    //! \brief polygon transformed rectangle specialization ctor
    //! \details Initializes a box and transforms it according to the provided
    //!          position and angle.
    //! \param [in] he_x x-axis half extent
    //! \param [in] he_y y-axis half extent
    //! \param [in] center Centroid position
    //! \param [in] angle Rotation angle (radians)
    explicit polygon (float he_x, float he_y, const math::vec2& center, float angle = 0.f);

    //!@{
    //! \brief polygon explicit vertices ctor
    //! \details Initialize a polygon from the list of vertices
    explicit polygon (const math::vec2& p1, const math::vec2& p2, const math::vec2& p3);
    explicit polygon (const math::vec2& p1, const math::vec2& p2,
                      const math::vec2& p3, const math::vec2& p4);
    explicit polygon (const math::vec2& p1, const math::vec2& p2, const math::vec2& p3,
                      const math::vec2& p4, const math::vec2& p5);
    //!@}

    //!@{ Shape properties
    ShapeType type (void) const override { return ShapeType::POLYGON; }
    math::vec2 get_centroid (void) const override { return centroid; }
    //!@}

    //! \brief Rotate the polygon
    //! \param [in] angle Rotation angle (radians)
    void rotate (float angle)
    {
        iso_transform xf(centroid, angle);
        for (size_t i = 0; i < count; i++)
        {
            vertices[i] = xf.to_world(vertices[i]);
            normals[i] = xf.rot.rotate(normals[i]);
        }
    }

    //! \brief Converts the polygon to world space
    //! \param [in] xf Transform
    void to_world (const iso_transform& xf) override
    {
        centroid = xf.to_world(centroid);
        for (size_t i = 0; i < count; i++)
        {
            vertices[i] = xf.to_world(vertices[i]);
            normals[i] = xf.rot.rotate(normals[i]);
        }
    }

    //! \brief Check if a point resides within the polygon (edge exclusive)
    //! \param [in] point Local point coordinates
    //! \returns True iff point is within the polygon
    bool contains (const math::vec2& point) const override;

    //! \brief Check if the polygon intersects with another shape
    //! \param [in] other Other shape to test
    //! \warning Before calling ensure both shapes are in the same coordinate space
    //! \returns True iff shapes intersect
    bool intersects_with (const ishape* other) const override;

    //! \brief Check if the polygon intersects with another shape
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other shape
    //! \param [out] mf Manifold containing resolution
    //! \warning Before calling ensure both shapes are in the same coordinate space
    //! \returns True iff intersecting
    bool intersects_with (const ishape* other, collision_manifold& mf) const override;

    //! \brief Compute an aabb surrounding the polygon
    //! \note aabb edges will be padded by \ref AABB_PADDING
    //! \returns Surrounding aabb
    aabb compute_aabb (void) const override;
    aabb compute_aabb (const iso_transform& xf) const override;

    //! \brief Compute the mass and analog data
    //! \param [in] density Density of the shape
    //! \returns Mass, centroid, and mass moment of inertia
    //! \see https://en.wikipedia.org/wiki/Centroid#Centroid_of_a_polygon
    //! \see https://en.wikipedia.org/wiki/List_of_moments_of_inertia
    mass_data compute_mass (float density) const override;

    //!@{ SAT support functions
    //! \brief Provides the min and max projection on the provided axis
    //! \param [in] axis Normalized axis
    math::vec2 project (const math::vec2& axis) const override
    {
        float min = axis.dot(vertices[0]);
        float max = min;
        for (size_t i = 1; i < count; i++)
        {
            float p = axis.dot(vertices[i]);
            if (p < min)
            {
                min = p;
            }
            else if (p > max)
            {
                max = p;
            }
        }

        return { min, max };
    }

    //! \brief Max separation of two polygons using the edge normals
    //! \details Similar to \ref farthest_point, where the edge normals are the
    //!          axes to test for separation.  Basically computes each Minkowski
    //!          difference vertex, and projects that onto the normal axis.  The
    //!          projection minimums are compared, with the largest of the values
    //!          being the result.  If this value is positive there exists an
    //!          axis of separation and therefore no collision.
    //! \param [in] other Polygon to test
    //! \returns Pair containing the max distance and corresponding edge index
    std::pair<float, int32> max_separation (const polygon& other) const
    {
        float sep_max = std::numeric_limits<float>::lowest();
        int32 edge_index = 0;
        for (size_t i = 0; i < count; i++)
        {
            float sep_axis = math::dot(normals[i], other.vertices[0] - vertices[i]);
            for (size_t j = 1; j < other.count; j++)
            {
                float sep = math::dot(normals[i], other.vertices[j] - vertices[i]);
                if (sep < sep_axis)
                {
                    sep_axis = sep;
                }
            }

            if (sep_axis > sep_max)
            {
                sep_max = sep_axis;
                edge_index = i;
            }
        }

        return std::make_pair(sep_max, edge_index);
    }

    //!@}

    //!@{ GJK support functions
    //! \brief Provides the first point in the array
    math::vec2 first_point (void) const override
    {
        return vertices[0];
    }

    //! \brief Retrieves the farthest point along the provided direction
    //! \param [in] d Direction to find the farthest point
    //! \note Provided direction requires no normalization
    math::vec2 farthest_point (const math::vec2& d) const override
    {
        size_t index = 0;
        float max_separation = math::dot(d, vertices[0]);
        for (size_t i = 1; i < count; i++)
        {
            float p = math::dot(d, vertices[i]);
            if (p > max_separation)
            {
                max_separation = p;
                index = i;
            }
        }

        return vertices[index];
    }
    //!@}

    //! \brief Check if the polygon intersects with another (edge exclusive)
    //! \param [in] other polygon structure
    //! \returns True iff intersecting
    bool intersects_with (const polygon& other) const noexcept
    {
        gjk test(this, &other);
        return test.intersects();
    }

    //! \brief Check if the polygon intersects with another (edge exclusive)
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other polygon structure
    //! \param [out] mf Manifold containing resolution
    //! \returns True iff intersecting
    bool intersects_with (const polygon& other, collision_manifold& mf) const noexcept;
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

//! \brief polygon stream output operator
std::ostream& operator<< (std::ostream&, const polygon&);

} // namespace physics
} // namespace physics
