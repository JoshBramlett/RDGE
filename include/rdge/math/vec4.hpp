//! \headerfile <rdge/math/vec4.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/14/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec3.hpp>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct vec4
//! \brief Four dimensional floating point vector
struct vec4
{
    float x = 0.f; //!< x-coordinate
    float y = 0.f; //!< y-coordinate
    float z = 0.f; //!< z-coordinate
    float w = 0.f; //!< w-coordinate

    //! \brief vec4 ctor
    //! \details Zero initialization via default member initialization
    constexpr vec4 (void) = default;

    //! \brief vec4 ctor
    //! \details Initialize vec4 from X, Y, Z and W values
    //! \param [in] px X-Coordinate
    //! \param [in] py Y-Coordinate
    //! \param [in] pz Z-Coordinate
    //! \param [in] pw W-Coordinate
    constexpr vec4 (float px, float py, float pz, float pw)
        : x(px), y(py), z(pz), w(pw)
    { }

    //! \brief vec4 ctor
    //! \param [in] scalar Value to initialize all elements
    constexpr vec4 (float scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar)
    { }

    //! \brief vec4 ctor
    //! \details Initialize vec4 from vec3 and W values.
    //! \param [in] v vec3 containing the x, y, and z coordinates
    //! \param [in] pw W-Coordinate
    constexpr vec4 (const vec3& v, float pw)
        : x(v.x), y(v.y), z(v.z), w(pw)
    { }

    //! \brief vec4 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    float& operator[] (uint8 index) noexcept;

    //! \brief vec4 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    const float& operator[] (uint8 index) const noexcept;

    //! \brief vec4 memberwise addition
    //! \param [in] rhs vec4 to add
    //! \returns Reference to self
    constexpr vec4& operator+= (const vec4& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    //! \brief vec4 memberwise subtraction
    //! \param [in] rhs vec4 to subtract
    //! \returns Reference to self
    constexpr vec4& operator-= (const vec4& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    //! \brief vec4 memberwise multiplication
    //! \param [in] rhs vec4 to multiply
    //! \returns Reference to self
    constexpr vec4& operator*= (const vec4& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }
};

//! \brief vec4 equality operator
//! \param [in] lhs Left side vec4 to compare
//! \param [in] rhs Right side vec4 to compare
//! \returns True iff vec4s are identical
constexpr bool operator== (const vec4& lhs, const vec4& rhs) noexcept
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
constexpr bool operator!= (const vec4& lhs, const vec4& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief vec4 unary negation operator
//! \param [in] value vec4 to negate
//! \returns Negated vec4
constexpr vec4 operator- (const vec4& value) noexcept
{
    return vec4(-value.x, -value.y, -value.z, -value.w);
}

//! \brief vec4 addition operator
//! \param [in] lhs Left side vec4 to add
//! \param [in] rhs Right side vec4 to add
//! \returns vec4 of added values
constexpr const vec4 operator+ (const vec4& lhs, const vec4& rhs) noexcept
{
    return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

//! \brief vec4 subtraction operator
//! \param [in] lhs Left side vec4 to subtract from
//! \param [in] rhs Right side vec4
//! \returns vec4 of subtracted values
constexpr const vec4 operator- (const vec4& lhs, const vec4& rhs) noexcept
{
    return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

//! \brief vec4 multiplication operator
//! \param [in] lhs Left side vec4 to multiply
//! \param [in] rhs Right side vec4 to multiply
//! \returns vec4 of multiplied values
constexpr const vec4 operator* (const vec4& lhs, const vec4& rhs) noexcept
{
    return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

//! \brief vec4 stream output operator
//! \param [in] os Output stream
//! \param [in] vec vec4 to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const vec4& vec);

} // namespace math
} // namespace rdge
