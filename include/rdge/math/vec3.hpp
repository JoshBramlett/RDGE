//! \headerfile <rdge/math/vec3.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/vec2.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//!@{ Forward declarations
struct mat4;
//!@}

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
    //! \param [in] px X-Coordinate
    //! \param [in] py Y-Coordinate
    //! \param [in] pz Z-Coordinate
    constexpr vec3 (float px, float py, float pz = 0.f)
        : x(px), y(py), z(pz)
    { }

    //! \brief vec3 ctor
    //! \param [in] scalar Value to initialize all elements
    constexpr vec3 (float scalar)
        : x(scalar), y(scalar), z(scalar)
    { }

    //! \brief vec3 ctor
    //! \details Initialize vec3 from vec2 and Z values.
    //! \param [in] v vec2 containing the x and y coordinates
    //! \param [in] pz Z-Coordinate
    constexpr vec3 (const vec2& v, float pz)
        : x(v.x), y(v.y), z(pz)
    { }

    //! \brief vec3 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    float& operator[] (uint8 index) noexcept;

    //! \brief vec3 subscript operator
    //! \param [in] index Index of containing element
    //! \returns Const reference to element
    float operator[] (uint8 index) const noexcept;

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

    //! \brief vec3 memberwise addition (x and y components only)
    //! \param [in] rhs vec2 to add
    //! \returns Reference to self
    constexpr vec3& operator+= (const vec2& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
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

    //! \brief Check if vector components are set to zero
    //! \returns True iff all components are zero
    constexpr bool is_zero (void) const noexcept
    {
        return x == 0.f && y == 0.f && z == 0.f;
    }

    //! \brief Get the xy coordinate vec2
    //! \returns vec2 containing the x and y coordinates
    constexpr vec2 xy (void) const noexcept
    {
        return { x, y };
    }

    //!@{
    //! \brief Apply a linear transformation to the vector
    //! \details Only linear transforms should be supplied (e.g. translation,
    //!          rotation, etc.).
    //! \param [in] xf Linear transformation
    //! \returns vec3 with applied transform
    vec3& transform (const mat4& xf) noexcept;
    vec3 transform (const mat4& xf) const noexcept;
    //!@}

    //! \brief Vector length (magnitude)
    //! \returns Length of the vector
    float length (void) const noexcept
    {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    //! \brief Normalize to a unit vector
    //! \returns Reference to self
    vec3& normalize (void) noexcept
    {
        if (!is_zero())
        {
            float inv_length = 1.f / length();
            *this *= inv_length;
        }

        return *this;
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
//! \returns True iff identical
constexpr bool operator== (const vec3& lhs, const vec3& rhs) noexcept
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y) && fp_eq(lhs.z, rhs.z);
}

//! \brief vec3 inequality operator
//! \returns True iff not identical
constexpr bool operator!= (const vec3& lhs, const vec3& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief vec3 unary negation operator
//! \returns vec3 of negated components
constexpr vec3 operator- (const vec3& value) noexcept
{
    return vec3(-value.x, -value.y, -value.z);
}

//! \brief vec3 addition operator
//! \returns vec3 of added components
constexpr vec3 operator+ (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

//! \brief vec3 subtraction operator
//! \returns vec3 of subtracted components
constexpr vec3 operator- (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

//! \brief vec3 multiplication operator
//! \returns vec3 of multiplied components
constexpr vec3 operator* (const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

//! \brief vec3 dot specialization
constexpr float dot (const vec3& lhs, const vec3& rhs) noexcept
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

//! \brief vec3 stream output operator
std::ostream& operator<< (std::ostream&, const vec3&);

} // namespace math

//! \brief vec3 string conversion
std::string to_string (const math::vec3&);

} // namespace rdge
