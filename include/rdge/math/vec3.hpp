//! \headerfile <rdge/math/vec3.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct vec3
//! \brief Three dimensional floating point vector
struct vec3
{
    float x = 0.f; //!< x-coordinate
    float y = 0.f; //!< y-coordinate
    float z = 0.f; //!< z-coordinate

    static const vec3 ZERO; //!< vec3 defined to { 0.f, 0.f, 0.f }
    static const vec3 X;    //!< vec3 defined to { 1.f, 0.f, 0.f }
    static const vec3 Y;    //!< vec3 defined to { 0.f, 1.f, 0.f }
    static const vec3 Z;    //!< vec3 defined to { 0.f, 0.f, 1.f }

    //! \brief vec3 ctor
    //! \details Zero initialization via default member initialization
    constexpr vec3 (void) = default;

    //! \brief vec3 ctor
    //! \details The optional z-coordinate if omitted will default to a value of 0.f,
    //!          which can be useful for 2D rendering where depth is not a concern.
    //! \param [in] px First element
    //! \param [in] py Second element
    //! \param [in] pz Third element
    constexpr vec3 (float px, float py, float pz = 0.f)
        : x(px), y(py), z(pz)
    { }

    //! \brief vec3 ctor
    //! \param [in] scalar Value to initialize all elements
    constexpr vec3 (float scalar)
        : x(scalar), y(scalar), z(scalar)
    { }

    //! \brief vec3 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    float& operator[] (uint8 index) noexcept;

    //! \brief vec3 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Const reference to element
    const float& operator[] (uint8 index) const noexcept;

    //! \brief vec3 memberwise addition
    //! \param [in] rhs vec3 to add
    //! \returns Reference to self
    constexpr vec3& operator+= (const vec3& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    //! \brief vec3 memberwise subtraction
    //! \param [in] rhs vec3 to subtract
    //! \returns Reference to self
    constexpr vec3& operator-= (const vec3& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    //! \brief vec3 memberwise multiplication
    //! \param [in] rhs vec3 to multiply
    //! \returns Reference to self
    constexpr vec3& operator*= (const vec3& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    //! \brief Vector length (magnitude)
    //! \returns Length of the vector
    float length (void) const noexcept
    {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    //! \brief Normalize vector components to [0,1]
    //! \returns Normalized vector
    vec3 normalize (void) const noexcept
    {
        float len = length();

        return (fp_eq(len, 0.f)) ? vec3::ZERO : vec3((x / len), (y / len), (z / len));
    }

    //! \brief Cross product
    //! \details Defined only in three dimensional space, the cross product produces a
    //!          vector that is perpendicular to both vectors used to calculate it.
    //!          For example, vectors with magnitude on the x and y planes produce
    //!          a vector on the z axis.
    //! \param [in] other Other vec3 used in the calculation
    //! \returns vec3 containing the cross product
    constexpr vec3 cross (const vec3& other) const noexcept
    {
        return vec3((y * other.z) - (z * other.y),
                    (z * other.x) - (x * other.z),
                    (x * other.y) - (y * other.x));
    }

    //! \brief Dot product
    //! \details Sum of the corresponding products within the containers.
    //! \param [in] other Other vec3 used in the calculation
    //! \returns Dot product of the vectors
    constexpr float dot (const vec3& other) const noexcept
    {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }
};

//! \brief vec3 equality operator
//! \param [in] lhs Left side vec3 to compare
//! \param [in] rhs Right side vec3 to compare
//! \returns True iff vectors are identical
constexpr bool operator== (const vec3& lhs, const vec3& rhs) noexcept
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y) && fp_eq(lhs.z, rhs.z);
}

//! \brief vec3 inequality operator
//! \param [in] lhs Left side vec3 to compare
//! \param [in] rhs Right side vec3 to compare
//! \returns True iff vectors are not identical
constexpr bool operator!= (const vec3& lhs, const vec3& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief vec3 unary negation operator
//! \param [in] value vec3 to negate
//! \returns Negated vec3
constexpr vec3 operator- (const vec3& value) noexcept
{
    return vec3(-value.x, -value.y, -value.z);
}

//! \brief vec3 addition operator
//! \param [in] lhs Left side vec3
//! \param [in] rhs Right side vec3
//! \returns vec3 of resultant values
constexpr vec3 operator+ (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

//! \brief vec3 subtraction operator
//! \param [in] lhs Left side vec3
//! \param [in] rhs Right side vec3
//! \returns vec3 of resultant values
constexpr vec3 operator- (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

//! \brief vec3 multiplication operator
//! \param [in] lhs Left side vec3
//! \param [in] rhs Right side vec3
//! \returns vec3 of resultant values
constexpr vec3 operator* (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

//! \brief vec3 stream output operator
//! \param [in] os Output stream
//! \param [in] value vec3 to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const vec3& value);

} // namespace math
} // namespace rdge
