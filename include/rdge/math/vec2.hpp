//! \headerfile <rdge/math/vec2.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

//! \struct vec2
//! \brief Two dimensional vector of floating point values
struct vec2
{
    //! \var x X-coordinate
    float x;
    //! \var y Y-coordinate
    float y;

    //! \brief Invalid
    //! \details Invalid vectors are often returned by failed functions.
    //! \returns An invalid vec2 object
    static constexpr vec2 Invalid (void) { return {NAN, NAN}; };

    //! \brief vec2 ctor
    //! \details Initialize vec4 to [0.0f,0.0f]
    constexpr vec2 (void)
        : x (0.0f)
        , y (0.0f)
    { }

    //! \brief vec2 ctor
    //! \details Initialize vec4 from X and Y values
    //! \param [in] x X-Coordinate
    //! \param [in] y Y-Coordinate
    constexpr vec2 (float x, float y)
        : x (x)
        , y (y)
    { }

    //! \brief vec2 Copy ctor
    //! \details Default-copyable
    constexpr vec2 (const vec2&) noexcept = default;

    //! \brief vec2 Move ctor
    //! \details Default-movable
    constexpr vec2 (vec2&&) noexcept = default;

    //! \brief vec2 Copy Assignment Operator
    //! \details Default-copyable
    vec2& operator= (const vec2&) noexcept = default;

    //! \brief vec2 Move Assignment Operator
    //! \details Default-movable
    vec2& operator= (vec2&&) noexcept = default;

    //! \brief Check if vector is valid
    //! \returns True if valid, false otherwise
    bool IsValid (void) const
    {
        // TODO: This cannot be marked constexpr because the MacOS uses a fork of
        //       libc++, rather than using libstdc++.  Once cross platform support
        //       is added, consider using preprocessor defines to make constexpr
        return !(std::isnan(x) && std::isnan(y));
    }

    vec2& add (const vec2& rhs);

    vec2& subtract (const vec2& rhs);

    vec2& multiply (const vec2& rhs);

    vec2& divide (const vec2& rhs);

    vec2& scale (float value);

    vec2& operator+= (const vec2& rhs);

    vec2& operator-= (const vec2& rhs);

    vec2& operator*= (const vec2& rhs);

    vec2& operator*= (float rhs);

    vec2& operator/= (const vec2& rhs);
};

inline bool operator== (const vec2& lhs, const vec2& rhs)
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y);
}

inline bool operator!= (const vec2& lhs, const vec2& rhs)
{
    return !fp_eq(lhs.x, rhs.x) || !fp_eq(lhs.y, rhs.y);
}

inline vec2 operator+ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline vec2 operator- (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline vec2 operator* (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2 operator/ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

std::ostream& operator<< (std::ostream& os, const vec2& vec);

} // namespace Math
} // namespace RDGE
