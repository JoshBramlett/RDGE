//! \headerfile <rdge/math/vec4.hpp>
//! \author Josh Bramlett
//! \version 0.0.4
//! \date 05/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

//! \struct vec4
//! \brief Four dimensional vector of floating point values
struct vec4
{
    //! \var x X-coordinate
    float x;
    //! \var y Y-coordinate
    float y;
    //! \var z Z-coordinate
    float z;
    //! \var w W-coordinate
    float w;

    //! \brief vec4 ctor
    //! \details Initialize vec4 to [0.0f,0.0f,0.0f,0.0f]
    constexpr vec4 (void)
        : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    { }

    //! \brief vec4 ctor
    //! \details Initialize vec4 from X, Y, Z and W values
    //! \param [in] x X-Coordinate
    //! \param [in] y Y-Coordinate
    //! \param [in] z Z-Coordinate
    //! \param [in] w W-Coordinate
    constexpr vec4 (float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    { }

    //! \brief vec4 Copy ctor
    //! \details Default-copyable
    constexpr vec4 (const vec4&) noexcept = default;

    //! \brief vec4 Move ctor
    //! \details Default-movable
    constexpr vec4 (vec4&&) noexcept = default;

    //! \brief vec4 Copy Assignment Operator
    //! \details Default-copyable
    vec4& operator= (const vec4&) noexcept = default;

    //! \brief vec4 Move Assignment Operator
    //! \details Default-movable
    vec4& operator= (vec4&&) noexcept = default;

    //! \brief Add vec4
    //! \param [in] value Value to add
    //! \returns Reference to this
    vec4& add (const vec4& value);

    //! \brief Subtract vec4
    //! \param [in] value Value to subtract
    //! \returns Reference to this
    vec4& subtract (const vec4& value);

    //! \brief Multiply by vec4
    //! \param [in] value Value to multiply
    //! \returns Reference to this
    vec4& multiply (const vec4& value);

    //! \brief Divide by vec4
    //! \param [in] value Value denominator
    //! \returns Reference to this
    vec4& divide (const vec4& value);

    //! \brief vec4 addition assignment operator
    //! \param [in] rhs Right side vec4 to add
    //! \returns Reference to this
    vec4& operator+= (const vec4& rhs);

    //! \brief vec4 subtraction assignment operator
    //! \param [in] rhs Right side vec4 to subtract from this
    //! \returns Reference to this
    vec4& operator-= (const vec4& rhs);

    //! \brief vec4 multiplication assignment operator
    //! \param [in] rhs Right side vec4 to multiply
    //! \returns Reference to this
    vec4& operator*= (const vec4& rhs);

    //! \brief vec4 division assignment operator
    //! \param [in] rhs Right side vec4 denominator
    //! \returns Reference to this
    vec4& operator/= (const vec4& rhs);
};

//! \brief vec4 equality operator
//! \param [in] lhs Left side vec4 to compare
//! \param [in] rhs Right side vec4 to compare
//! \returns True iff vec4s are identical
constexpr bool operator== (const vec4& lhs, const vec4& rhs)
{
    return fp_eq(lhs.x, rhs.x) &&
           fp_eq(lhs.y, rhs.y) &&
           fp_eq(lhs.z, rhs.z) &&
           fp_eq(lhs.w, rhs.w);
}

//! \brief vec4 inequality operator
//! \param [in] lhs Left side vec4 to compare
//! \param [in] rhs Right side vec4 to compare
//! \returns True iff vec4s are not identical
constexpr bool operator!= (const vec4& lhs, const vec4& rhs)
{
    return !(lhs == rhs);
}

//! \brief vec4 addition operator
//! \param [in] lhs Left side vec4 to add
//! \param [in] rhs Right side vec4 to add
//! \returns vec4 of added values
constexpr const vec4 operator+ (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

//! \brief vec4 subtraction operator
//! \param [in] lhs Left side vec4 to subtract from
//! \param [in] rhs Right side vec4
//! \returns vec4 of subtracted values
constexpr const vec4 operator- (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

//! \brief vec4 multiplication operator
//! \param [in] lhs Left side vec4 to multiply
//! \param [in] rhs Right side vec4 to multiply
//! \returns vec4 of multiplied values
constexpr const vec4 operator* (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

//! \brief vec4 division operator
//! \param [in] lhs Left side vec4 numerator
//! \param [in] rhs Right side vec4 denominator
//! \returns vec4 of divided values
constexpr const vec4 operator/ (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

//! \brief vec4 stream output operator
//! \param [in] os Output stream
//! \param [in] vec vec4 to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const vec4& vec);

} // namespace Math
} // namespace RDGE
