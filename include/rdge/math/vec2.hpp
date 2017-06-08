//! \headerfile <rdge/math/vec2.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/intrinsics.hpp>

#include <cassert>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

template <typename T, typename = void>
struct vec2_t;

//! \struct vec2_t
//! \brief Arithmetic typed two element container
template <typename T>
struct vec2_t <T, std::enable_if_t<std::is_arithmetic<T>::value>>
{
    //! \typedef value_type vec2_t type
    using value_type = T;

    //! \brief Number of elements
    //! \returns size_t type
    constexpr size_t size (void) const
    {
        return 2;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    //! \union Member access through x,y or w.h
    union
    {
        struct { T x, y; }; //!< x/y coordinates
        struct { T w, h; }; //!< width/height
    };

#pragma GCC diagnostic pop

    //! \brief vec2_t ctor
    //! \details Zero initialization
    constexpr vec2_t (void)
        : x(0), y(0)
    { }

    //! \brief vec2_t ctor
    //! \param [in] px First element
    //! \param [in] py Second element
    constexpr vec2_t (T px, T py)
        : x(px), y(py)
    { }

    //! \brief vec2_t ctor
    //! \param [in] scalar Value to initialize all elements
    constexpr vec2_t (T scalar)
        : x(scalar), y(scalar)
    { }

    //! \brief vec2_t User-defined underlying type conversion
    //! \note The conversion is explicit so it must be used with direct
    //!       initialization or explicit conversions.
    //! \returns Copy with casted underlying types
    template <typename U>
    explicit constexpr operator vec2_t<U> (void) const
    {
        return vec2_t<U>(static_cast<U>(x), static_cast<U>(y));
    }

    //! \brief vec2_t subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    constexpr T& operator[] (uint8 index)
    {
        assert(index < size());
        return (&x)[index];
    }

    //! \brief vec2_t subscript operator
    //! \param [in] index Index of containing element
    //! \returns Const reference to element
    constexpr const T& operator[] (uint8 index) const
    {
        assert(index < size());
        return (&x)[index];
    }

    //! \brief vec2_t memberwise addition
    //! \param [in] rhs vec2_t to add
    //! \returns Reference to self
    template <typename U>
    constexpr vec2_t<T>& operator+= (const vec2_t<U>& rhs) noexcept
    {
        x += static_cast<T>(rhs.x);
        y += static_cast<T>(rhs.y);
        return *this;
    }

    //! \brief vec2_t memberwise addition
    //! \param [in] scalar Value applied to all elements
    //! \returns Reference to self
    template <typename U>
    constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>&>
    operator+= (U scalar) noexcept
    {
        x += static_cast<T>(scalar);
        y += static_cast<T>(scalar);
        return *this;
    }

    //! \brief vec2_t memberwise subtraction
    //! \param [in] rhs vec2_t to subtract
    //! \returns Reference to self
    template <typename U>
    constexpr vec2_t<T>& operator-= (const vec2_t<U>& rhs) noexcept
    {
        x -= static_cast<T>(rhs.x);
        y -= static_cast<T>(rhs.y);
        return *this;
    }

    //! \brief vec2_t memberwise subtraction
    //! \param [in] scalar Value applied to all elements
    //! \returns Reference to self
    template <typename U>
    constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>&>
    operator-= (U scalar) noexcept
    {
        x -= static_cast<T>(scalar);
        y -= static_cast<T>(scalar);
        return *this;
    }

    //! \brief vec2_t memberwise multiplication
    //! \param [in] rhs vec2_t to multiply
    //! \returns Reference to self
    template <typename U>
    constexpr vec2_t<T>& operator*= (const vec2_t<U>& rhs) noexcept
    {
        x *= static_cast<T>(rhs.x);
        y *= static_cast<T>(rhs.y);
        return *this;
    }

    //! \brief vec2_t memberwise multiplication
    //! \param [in] scalar Value applied to all elements
    //! \returns Reference to self
    template <typename U>
    constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>&>
    operator*= (U scalar) noexcept
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        return *this;
    }

    //! \brief Check if vector components are set to zero
    //! \returns True iff all components are zero
    constexpr bool is_zero (void) const noexcept
    {
        return x == 0 && y == 0;
    }

    //! \brief Vector length (magnitude)
    //! \returns Length of the vector
    float length (void) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'length' is only available for floating point types");
        return std::sqrt((x * x) + (y * y));
    }

    //! \brief Normalize to a unit vector
    //! \returns Reference to self
    vec2_t<T>& normalize (void) noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'normalize' is only available for floating point types");
        if (!is_zero())
        {
            float inv_length = 1.f / length();
            *this *= inv_length;
        }

        return *this;
    }

    //! \brief Normalize to a unit vector
    //! \returns Normalized vector
    const vec2_t<T> normalize (void) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'normalize' is only available for floating point types");
        if (!is_zero())
        {
            float inv_length = 1.f / length();
            return *this * inv_length;
        }

        return { 0.f, 0.f };
    }

    //! \brief Dot product
    //! \details Sum of the corresponding products within the containers.
    //! \param [in] other Other vec2_t used in the calculation
    //! \returns Dot product of the vectors
    constexpr float dot (const vec2_t<T>& other) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'dot' is only available for floating point types");
        return (x * other.x) + (y * other.y);
    }

    //! \brief Compute the dot product with itself
    //! \details Represents the length squared, and is useful for avoiding
    //!          an expensive sqrt call when comparing lengths.
    //! \returns Length squared
    constexpr float self_dot (void) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'self_dot' is only available for floating point types");
        return (x * x) + (y * y);
    }

    //! \brief Get a perpendicular vector (rotated clockwise)
    //! \returns Perpendicular vector
    constexpr vec2_t<T> perp (void) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'perp' is only available for floating point types");
        return vec2_t<T>(-y, x);
    }

    //! \brief Get a perpendicular vector (rotated counter-clockwise)
    //! \returns Perpendicular vector
    constexpr vec2_t<T> perp_ccw (void) const noexcept
    {
        static_assert(std::is_floating_point<T>::value,
                      "'perp_ccw' is only available for floating point types");
        return vec2_t<T>(y, -x);
    }
};

//! \brief vec2_t equality operator
//! \returns True iff identical
template <typename T>
constexpr bool operator== (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

//! \brief vec2_t inequality operator
//! \returns True iff not identical
template <typename T>
constexpr bool operator!= (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief vec2_t unary negation operator
//! \returns vec2_t of negated components
template <typename T>
constexpr vec2_t<T> operator- (const vec2_t<T>& value) noexcept
{
    return vec2_t<T>(-value.x, -value.y);
}

//! \brief vec2_t addition operator
//! \returns vec2_t of added components
template <typename T>
constexpr vec2_t<T> operator+ (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

//! \brief vec2_t scalar addition operator
//! \returns vec2_t of components after addition of the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>>
operator+ (const vec2_t<T>& vec, U scalar) noexcept
{
    return vec2_t<T>(vec.x + static_cast<T>(scalar), vec.y + static_cast<T>(scalar));
}

//! \brief vec2_t subtraction operator
//! \returns vec2_t of subtracted components
template <typename T>
constexpr vec2_t<T> operator- (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

//! \brief vec2_t scalar subtraction operator
//! \returns vec2_t of components after subtraction of the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>>
operator- (const vec2_t<T>& vec, U scalar) noexcept
{
    return vec2_t<T>(vec.x - static_cast<T>(scalar), vec.y - static_cast<T>(scalar));
}

//! \brief vec2_t multiplication operator
//! \returns vec2_t of multiplied components
template <typename T>
constexpr vec2_t<T> operator* (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

//! \brief vec2_t scalar multiplication operator
//! \returns vec2_t of components after multiplication of the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>>
operator* (const vec2_t<T>& vec, U scalar) noexcept
{
    return vec2_t<T>(vec.x * static_cast<T>(scalar), vec.y * static_cast<T>(scalar));
}

//! \brief vec2_t scalar multiplication operator
//! \returns vec2_t of components after multiplication of the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>>
operator* (U scalar, const vec2_t<T>& vec) noexcept
{
    return vec * scalar;
}

//! \brief Intrinsic vec2_t abs specialization
//! \returns vec2_t of abs components
template <typename T>
constexpr vec2_t<T> abs (const vec2_t<T>& vec) noexcept
{
    return vec2_t<T>(math::abs(vec.x), math::abs(vec.y));
}

//! \brief Intrinsic vec2_t clamp specialization
//! \returns vec2_t of clamped components
template <typename T>
constexpr vec2_t<T> clamp (const vec2_t<T>& vec, T lbound, T ubound) noexcept
{
    return vec2_t<T>(math::clamp(vec.x, lbound, ubound),
                     math::clamp(vec.y, lbound, ubound));
}

//! \brief vec2_t dot specialization
//! \returns Dot product of vec2_t components
template <typename T>
constexpr T dot (const vec2_t<T>& a, const vec2_t<T>& b) noexcept
{
    static_assert(std::is_floating_point<T>::value,
                  "'dot' is only available for floating point types");
    return (a.x * b.x) + (a.y * b.y);
}

//! \brief vec2_t perp_dot product specialization
//! \details The dot product of the perpendicular of vector a with vector b.
//!          The result represents the area of the parallelogram created by
//!          the vectors.  Also referred to as the 2D analog of the cross
//!          product.  Other properties include:
//!            a) result > 0 iff b is CCW from a
//!            b) result < 0 iff b is CW from a
//!            c) result == 0 iff a and b are parallel
//! \returns Perpendicular dot product scalar
template <typename T>
constexpr T perp_dot (const vec2_t<T>& a, const vec2_t<T>& b) noexcept
{
    static_assert(std::is_floating_point<T>::value,
                  "'perp_dot' is only available for floating point types");
    return (a.x * b.y) - (a.y * b.x);
}

//! \brief vec2_t vector triple product specialization
//! \details The cross product of vector a with the cross product of vectors
//!          b and c.  e.g. [a x (b x c)].  Known by the mnemonic BAC - CAB.
//! \returns Vector triple product
template <typename T>
constexpr vec2_t<T> triple (const vec2_t<T>& a, const vec2_t<T>& b, const vec2_t<T>& c) noexcept
{
    static_assert(std::is_floating_point<T>::value,
                  "'triple' is only available for floating point types");
    return (b * dot(a, c)) - (c * dot(a, b));
}

//! \brief vec2_t stream output operator
//! \returns Output stream
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const vec2_t<T>& value)
{
    return os << "[" << value.x << ", " << value.y << "]";
}

using vec2   = vec2_t<float>;  //!< Default floating point type
using uivec2 = vec2_t<uint32>; //!< Unsigned (useful for size types)
using ivec2  = vec2_t<int32>;  //!< Signed (useful for screen coordinates)

} // namespace math
} // namespace rdge
