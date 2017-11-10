//! \headerfile <rdge/physics/aabb.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017

// TODO Many functions are missing and should be added on an as-needed basis.
//
// Other implementations:
//
// http://docs.godotengine.org/en/stable/classes/class_aabb.html
// https://github.com/erincatto/Box2D/blob/master/Box2D/Box2D/Collision/b2Collision.h#L162

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <SDL_assert.h>

#include <algorithm> // min, max

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//!@{ Forward declarations
struct collision_manifold;
//!@}

//! \struct aabb
//! \brief Floating point structure defining an axis aligned bounding box
//! \details Structure contains two opposite points of a rectangle by grouping the
//!          min(x,y) and max(x,y) together.  All collision checks do not include
//!          the edges when checking, which means AABBs that are equal or share
//!          an edge/corner are not regarded to collide.
//! \warning Manually modify lo and hi bounds at your own risk.  Methods called
//!          on an invalid container will yield spurious results.
struct aabb
{
    math::vec2 lo; //!< Lower x and y coordinate position
    math::vec2 hi; //!< Higher x and y coordinate position

    //! \brief aabb default ctor
    //! \details Zero initialization
    constexpr aabb (void)
        : lo(0.f, 0.f), hi(0.f, 0.f)
    { }

    //! \brief aabb ctor
    //! \details Initialize aabb from min/max coordinates.
    //! \param [in] plo Lower coordinate
    //! \param [in] phi Higher coordinate
    constexpr aabb (const math::vec2& plo, const math::vec2& phi)
        : lo(std::min(plo.x, phi.x), std::min(plo.y, phi.y))
        , hi(std::max(plo.x, phi.x), std::max(plo.y, phi.y))
    { }

    //! \brief aabb ctor
    //! \details Initialize aabb from origin and width/height values.
    //! \param [in] origin Origin (lower left)
    //! \param [in] width aabb width
    //! \param [in] height aabb height
    constexpr aabb (const math::vec2& origin, float width, float height)
        : lo(origin)
        , hi(origin.x + width, origin.y + height)
    { }

    //! \brief Verify the bounds are sorted
    //! \returns True iff sorted
    constexpr bool is_valid (void) const noexcept
    {
        return (hi.x >= lo.x) && (hi.y >= lo.y);
    }

    //!@{ \brief Size values
    constexpr float width (void) const noexcept { return hi.x - lo.x; }
    constexpr float height (void) const noexcept { return hi.y - lo.y; }
    //!@}

    //!@{ \brief Edge values
    constexpr float top (void) const noexcept { return hi.y; }
    constexpr float left (void) const noexcept { return lo.x; }
    constexpr float bottom (void) const noexcept { return lo.y; }
    constexpr float right (void) const noexcept { return hi.x; }
    //!@}

    //!@{ \brief Corner values
    constexpr math::vec2 top_left (void) const noexcept { return { left(), top() }; }
    constexpr math::vec2 top_right (void) const noexcept { return { right(), top() }; }
    constexpr math::vec2 bottom_left (void) const noexcept { return { left(), bottom() }; }
    constexpr math::vec2 bottom_right (void) const noexcept { return { right(), bottom() }; }
    //!@}

    //! \brief Merge another aabb with this one
    //! \param [in] other aabb to merge
    //! \returns Reference to self
    aabb& merge (const aabb& other) noexcept
    {
        SDL_assert(other.is_valid());

        lo.x = std::min(lo.x, other.lo.x);
        lo.y = std::min(lo.y, other.lo.y);
        hi.x = std::max(hi.x, other.hi.x);
        hi.y = std::max(hi.y, other.hi.y);
        return *this;
    }

    //! \brief Extend the lo and hi coordinates by the provided value
    //! \param [in] amount Amount to extend
    aabb& fatten (float amount) noexcept
    {
        lo -= amount;
        hi += amount;
        return *this;
    }

    //! \brief Scale dimensions
    //! \param [in] amount Amount to scale
    aabb& scale (float amount) noexcept
    {
        lo *= amount;
        hi *= amount;
        return *this;
    }

    //! \brief Get the perimeter length
    //! \returns Perimeter length
    constexpr float perimeter (void) const noexcept
    {
        return 2.f * ((hi.x - lo.x) + (hi.y - lo.y));
    }

    //! \brief Get the calculated center of the aabb
    //! \returns Center point
    constexpr math::vec2 centroid (void) const noexcept
    {
        return (lo + hi) * 0.5f;
    }

    //! \brief Get the distance between an edge and the centroid
    //! \returns Half-widths
    constexpr math::vec2 half_extent (void) const noexcept
    {
        return (hi - lo) * 0.5f;
    }

    //! \brief Check if a point resides within the aabb (edge exclusive)
    //! \param [in] point Point coordinates
    //! \returns True iff point is within the aabb
    constexpr bool contains (const math::vec2& point) const noexcept
    {
        return point.x > left() &&
               point.x < right() &&
               point.y > bottom() &&
               point.y < top();
    }

    //! \brief Check if an aabb resides within this aabb (edge exclusive)
    //! \param [in] other aabb structure
    //! \returns True iff other is within the aabb
    constexpr bool contains (const aabb& other) const noexcept
    {
        return other.left() > left() &&
               other.right() < right() &&
               other.bottom() > bottom() &&
               other.top() < top();
    }

    //! \brief Check if the aabb intersects with another (edge exclusive)
    //! \param [in] other aabb structure
    //! \returns True iff intersecting
    constexpr bool intersects_with (const aabb& other) const noexcept
    {
        return other.left() < right() &&
               left() < other.right() &&
               other.bottom() < top() &&
               bottom() < other.top();
    }

    //! \brief Check if the aabb intersects with another (edge exclusive)
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other aabb structure
    //! \param [out] mf Manifold containing resolution
    //! \returns True iff intersecting
    bool intersects_with (const aabb& other, collision_manifold& mf) const noexcept;

    //! \brief Construct an aabb by merging two aabbs
    //! \param [in] a First aabb
    //! \param [in] b Second aabb
    //! \returns Merged aabb
    static aabb merge (const aabb& a, const aabb& b)
    {
        SDL_assert(a.is_valid());
        SDL_assert(b.is_valid());

        return aabb({ std::min(a.lo.x, b.lo.x), std::min(a.lo.y, b.lo.y) },
                    { std::max(a.hi.x, b.hi.x), std::max(a.hi.y, b.hi.y) });
    }
};

//! \brief aabb equality operator
//! \returns True iff identical
constexpr bool operator== (const aabb& lhs, const aabb& rhs) noexcept
{
    return (lhs.lo == rhs.lo) && (lhs.hi == rhs.hi);
}

//! \brief aabb inequality operator
//! \returns True iff not identical
constexpr bool operator!= (const aabb& lhs, const aabb& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief aabb stream output operator
inline std::ostream& operator<< (std::ostream& os, const aabb& value)
{
    return os << "[ " << value.lo << ", " << value.hi << " ]";
}

} // namespace physics
} // namespace rdge
