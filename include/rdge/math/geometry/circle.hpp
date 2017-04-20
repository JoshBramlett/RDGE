//! \headerfile <rdge/math/geometry/circle.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/19/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/collision.hpp>

#include <algorithm>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

struct shape
{
    enum class Type : uint8
    {
        CIRCLE = 1,
        POLYGON
    };

    //https://github.com/erincatto/Box2D/blob/master/Box2D/Box2D/Collision/Shapes/b2Shape.h

    virtual ~shape (void) = default;
};

//! \struct circle
//! \brief Floating point structure defining an axis aligned bounding box
//! \details Structure contains two opposite points of a rectangle by grouping the
//!          min(x,y) and max(x,y) together.  All collision checks do not include
//!          the edges when checking, which means AABBs that are equal or share
//!          an edge/corner are not regarded to collide.
//! \warning Manually modify lo and hi bounds at your own risk.  Methods called
//!          on an invalid container will yield spurious results.
struct circle : public shape
{
    vec2  pos;          //!< Position at center
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
    constexpr circle (const vec2& px, float r)
        : pos(px), radius(r)
    { }

    constexpr shape::Type type (void) const noexcept
    {
        return shape::Type::CIRCLE;
    }

    //! \brief Check if a point resides within the circle (edge exclusive)
    //! \param [in] point Point coordinates
    //! \returns True iff point is within the circle
    constexpr bool contains (const vec2& point) const noexcept
    {
        return (point - pos).self_dot() < square(radius);
    }

    //! \brief Check if the circle intersects with another (edge exclusive)
    //! \param [in] other circle structure
    //! \returns True iff intersecting
    constexpr bool intersects_with (const circle& other) const noexcept
    {
        return (other.pos - pos).self_dot() < square(radius + other.radius);
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

//! \brief aabb stream output operator
inline std::ostream& operator<< (std::ostream& os, const circle& c)
{
    return os << "[ " << c.pos << ", r=" << c.radius << " ]";
}

} // namespace math
} // namespace rdge
