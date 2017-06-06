//! \headerfile <rdge/physics/shapes/circle.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/19/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/collision.hpp>

#include <algorithm>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \struct circle
//! \brief Floating point structure defining a circle
//! \details Structure contains the position and radius
struct circle : public ishape
{
    math::vec2 pos;     //!< Position at center
    float radius = 0.f; //!< Circle radius

    //! \brief circle default ctor
    //! \details Zero initialization
    constexpr circle (void)
        : pos(0.f, 0.f), radius(0.f)
    { }

    //! \brief circle ctor
    //! \details Initialize circle from position and radius
    //! \param [in] px Position
    //! \param [in] r Radius
    constexpr circle (const math::vec2& px, float r)
        : pos(px), radius(r)
    { }

    //! \returns Underlying type
    ShapeType type (void) const override { return ShapeType::CIRCLE; }

    //! \brief Check if a point resides within the circle (edge exclusive)
    //! \param [in] point Local point coordinates
    //! \returns True iff point is within the circle
    bool contains (const math::vec2& point) const override
    {
        return (point - pos).self_dot() < math::square(radius);
    }

    //! \brief Compute an aabb surrounding the circle
    //! \warning Resultant value may still need to be converted to world space
    //! \returns Surrounding aabb
    aabb compute_aabb (void) const override
    {
        return aabb({ pos.x - radius, pos.y - radius },
                    { pos.x + radius, pos.y + radius });
    }

    //! \brief Compute the mass and analog data
    //! \param [in] density Density of the shape
    //! \returns Mass, centroid, and mass moment of inertia
    //! \see https://en.wikipedia.org/wiki/List_of_moments_of_inertia
    mass_data compute_mass (float density) const override
    {
        mass_data result;
        result.centroid = pos;
        result.mass = density * math::PI * math::square(radius);

        // circle mass moment of inertia: (mass * radius^2) / 2
        // parallel axis theorem: (inertia at center of mass) + (mass * distance^2)
        result.mmoi = (0.5f * result.mass * math::square(radius)) + // mmoi
                      (result.mass * pos.self_dot());               // parallel axis

        return result;
    }

    //!@{ GJK support functions
    //! \brief Provides the topmost point
    math::vec2 first_point (void) const override { return { pos.x + radius, pos.y }; }

    //! \brief Retrieves the farthest point along the provided direction
    //! \param [in] d Direction to find the farthest point
    math::vec2 farthest_point (const math::vec2& d) const override
    {
        float theta = atan2(d.y, d.x);
        return { pos.x + (radius * cosf(theta)),
                 pos.y + (radius * sinf(theta)) };
    }
    //!@}

    //! \brief Check if the circle intersects with another (edge exclusive)
    //! \param [in] other circle structure
    //! \returns True iff intersecting
    constexpr bool intersects_with (const circle& other) const noexcept
    {
        return (other.pos - pos).self_dot() < math::square(radius + other.radius);
    }

    //! \brief Check if the circle intersects with another (edge exclusive)
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other circle structure
    //! \param [out] mf Manifold containing resolution
    //! \returns True iff intersecting
    bool intersects_with (const circle& other, collision_manifold& mf) const noexcept;
};

//! \brief circle equality operator
//! \returns True iff identical
constexpr bool operator== (const circle& lhs, const circle& rhs) noexcept
{
    return (lhs.pos == rhs.pos) && (lhs.radius == rhs.radius);
}

//! \brief circle inequality operator
//! \returns True iff not identical
constexpr bool operator!= (const circle& lhs, const circle& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief circle stream output operator
inline std::ostream& operator<< (std::ostream& os, const circle& c)
{
    return os << "[ " << c.pos << ", r=" << c.radius << " ]";
}

} // namespace physics
} // namespace rdge
