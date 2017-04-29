//! \headerfile <rdge/math/transform.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/25/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct transformation
//! \brief Container a translation and rotation
struct transformation
{
    //! \brief transformation default ctor
    constexpr transformation (void) = default;

    //! \brief transformation ctor
    //! \param [in] p Position
    //! \param [in] r Rotation
    constexpr transformation (const vec2& p, const rotation& r)
        : pos(p), rot(r)
    { }

    //! \brief transformation ctor
    //! \param [in] p Position
    //! \param [in] radians Angle
    constexpr transformation (const vec2& p, float radians)
        : pos(p), rot(rotation::from_angle(radians))
    { }

    //! \brief Set the identity transformation
    constexpr void set_identity (void) noexcept
    {
        pos = { 0.f, 0.f };
        rot.set_identity();
    }

    //! \brief Set the rotation angle
    //! \param [in] radians Angle
    constexpr void set_angle (float radians) noexcept
    {
        rot = rotation::from_angle(radians);
    }

    vec2     pos; //!< Position
    rotation rot; //!< Rotation
};

} // namespace math
} // namespace rdge
