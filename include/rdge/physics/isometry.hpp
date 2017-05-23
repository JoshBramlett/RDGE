//! \headerfile <rdge/physics/isometry.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/25/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/aabb.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \struct rotation
//! \brief 2D rotation transformation
//! \details Could be considered a 2D rotation matrix, but takes advantage of
//!          the property of the identity and only stores the sine/cosine of
//!          the angle.  The rotation matrix is defined as
//!          R(theta) = | cos(theta) -sin(theta) |
//!                     | sin(theta)  cos(theta) |
struct rotation
{
    float c = 0.f; //!< cosine
    float s = 0.f; //!< sine

    //! \brief rotation default ctor
    rotation (void) = default;

    //! \brief rotation ctor
    //! \param [in] theta Angle in radians
    explicit rotation (float theta)
        : c(std::cosf(theta)), s(std::sinf(theta))
    { }

    //! \brief Get the rotation angle
    //! \returns Angle in radians
    float angle (void) const noexcept
    {
        return std::atan2f(s, c);
    }

    //! \brief Rotate a vector
    //! \param [in] vec Vector to rotate
    //! \returns Vector rotated about the angle
    math::vec2 rotate (const math::vec2& vec) const noexcept
    {
        return { c * vec.x - s * vec.y,
                 s * vec.x + c * vec.y };
    }

    //! \brief Rotate a vector using the inverted rotation
    //! \details For rotations the inverse matrix is the same as the transpose.
    //! \param [in] vec Vector to rotate
    //! \returns Vector rotated about the negative angle
    math::vec2 inv_rotate (const math::vec2& vec) const noexcept
    {
        return { c * vec.x + s * vec.y,
                 -s * vec.x + c * vec.y };
    }

    //! \brief Get the x-axis of the rotation identity
    //! \returns Vector of cos/sin values
    math::vec2 x_axis (void) const noexcept
    {
        return math::vec2(c, s);
    }

    //! \brief Get the y-axis of the rotation identity
    //! \returns Vector of -sin/cos values
    math::vec2 y_axis (void) const noexcept
    {
        return math::vec2(-s, c);
    }

    //! \brief Get the identity rotation
    //! \returns Identity rotation
    static rotation identity (void) noexcept
    {
        rotation result;
        result.c = 1.f;
        result.s = 0.f;

        return result;
    }
};

//! \struct iso_transform
//! \brief 2D linear transformation that preserves the solid body shape
//! \details Includes translation and rotation transformations built specifically
//!          for 2D physics simulation.
//! \see http://www.euclideanspace.com/maths/geometry/affine/index.htm
struct iso_transform
{
    math::vec2 pos; //!< Position (translation)
    rotation   rot; //!< Rotation

    //! \brief iso_transform default ctor
    iso_transform (void) = default;

    //! \brief iso_transform ctor
    //! \param [in] p Position
    //! \param [in] r Rotation
    explicit iso_transform (const math::vec2& p, const rotation& r)
        : pos(p), rot(r)
    { }

    //! \brief iso_transform ctor
    //! \param [in] p Position
    //! \param [in] theta Angle in radians
    explicit iso_transform (const math::vec2& p, float theta)
        : pos(p), rot(theta)
    { }

    //! \brief Set the rotation angle
    //! \param [in] theta Angle in radians
    void set_angle (float theta) noexcept
    {
        rot = rotation(theta);
    }

    //! \brief Convert a point in world space to local space
    //! \param [in] point Point to convert
    //! \returns Point in local coordinates
    math::vec2 to_local (const math::vec2& point) const noexcept
    {
        return rot.inv_rotate(point - pos);
    }

    //! \brief Convert a point in local space to world space
    //! \param [in] point Point to convert
    //! \returns Point in world coordinates
    math::vec2 to_world (const math::vec2& point) const noexcept
    {
        return rot.rotate(point) + pos;
    }

    //! \brief Convert an aabb in local space to world space
    //! \param [in] box aabb to convert
    //! \returns aabb in world coordinates
    aabb to_world (const aabb& box) const noexcept
    {
        return aabb(to_world(box.lo), to_world(box.hi));
    }

    //! \brief Get the identity isometric transformation
    //! \returns Identity isometric transformation
    static iso_transform identity (void) noexcept
    {
        iso_transform result;
        result.pos = { 0.f, 0.f };
        result.rot = rotation::identity();

        return result;
    }
};

} // namespace physics
} // namespace rdge
