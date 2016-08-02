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
        : x(0.0f), y(0.0f)
    { }

    //! \brief vec2 ctor
    //! \details Initialize vec4 from X and Y values
    //! \param [in] x X-Coordinate
    //! \param [in] y Y-Coordinate
    constexpr vec2 (float x, float y)
        : x(x), y(y)
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

    constexpr size_t length (void) const
    {
        return 2;
    }

    constexpr float operator[] (RDGE::UInt8 index) const
    {
        return (index == 0) ? x : y;
    }

    //! \brief Check if vector is valid
    //! \returns True if valid, false otherwise
    bool IsValid (void) const
    {
        // TODO: This cannot be marked constexpr because the MacOS uses a fork of
        //       libc++, rather than using libstdc++.  Once cross platform support
        //       is added, consider using preprocessor defines to make constexpr
        //       (fyi the isnan call is not constexpr)
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

    //! \brief vec2 division assignment operator
    //! \param [in] rhs Right side vec2 denominator
    //! \returns Reference to this
    vec2& operator/= (const vec2& rhs);
};

//! \brief vec2 equality operator
//! \param [in] lhs Left side vec2 to compare
//! \param [in] rhs Right side vec2 to compare
//! \returns True iff vec2s are identical
constexpr bool operator== (const vec2& lhs, const vec2& rhs)
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y);
}

//! \brief vec2 inequality operator
//! \param [in] lhs Left side vec2 to compare
//! \param [in] rhs Right side vec2 to compare
//! \returns True iff vec2s are not identical
constexpr bool operator!= (const vec2& lhs, const vec2& rhs)
{
    return !(lhs == rhs);
}

//! \brief vec2 addition operator
//! \param [in] lhs Left side vec2 to add
//! \param [in] rhs Right side vec2 to add
//! \returns vec2 of added values
constexpr const vec2 operator+ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

//! \brief vec2 subtraction operator
//! \param [in] lhs Left side vec2 to subtract from
//! \param [in] rhs Right side vec2
//! \returns vec2 of subtracted values
constexpr const vec2 operator- (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

//! \brief vec2 multiplication operator
//! \param [in] lhs Left side vec2 to multiply
//! \param [in] rhs Right side vec2 to multiply
//! \returns vec2 of multiplied values
constexpr const vec2 operator* (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

//! \brief vec2 division operator
//! \param [in] lhs Left side vec2 numerator
//! \param [in] rhs Right side vec2 denominator
//! \returns vec2 of divided values
constexpr const vec2 operator/ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

//! \brief vec2 stream output operator
//! \param [in] os Output stream
//! \param [in] vec vec2 to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const vec2& vec);

} // namespace Math
} // namespace RDGE
