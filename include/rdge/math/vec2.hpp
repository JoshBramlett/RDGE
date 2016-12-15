//! \headerfile <rdge/math/vec2.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

//template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value>>
//struct vec2_t;

//! \struct vec2_t
//! \brief Templated two element container
template <typename T>
struct vec2_t
{
    //! \typedef value_type vec2_t type
    using value_type = T;

    //! \brief Number of elements
    //! \returns size_t type
    constexpr size_t length (void) const noexcept
    {
        return 2;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    //! \union Member access through x,y or w.h
    union
    {
        struct { T x, y; };
        struct { T w, h; };
    };

#pragma GCC diagnostic pop

    //! \brief vec2_t ctor
    //! \details Initialize vector to [0,0]
    constexpr vec2_t (void)
        : x(0), y(0)
    { }

    //! \brief vec2_t ctor
    //! \param [in] px First value
    //! \param [in] py Second value
    explicit constexpr vec2_t (T px, T py)
        : x(px), y(py)
    { }

    //! \brief vec2_t ctor
    //! \param [in] scalar Value to initialize all elements
    explicit constexpr vec2_t (T scalar)
        : x(scalar), y(scalar)
    { }

    //! \brief vec2_t dtor
    ~vec2_t (void) noexcept = default;

    //! \brief vec2_t Copy ctor
    //! \details Default-copyable
    constexpr vec2_t<T> (const vec2_t<T>&) = default;

    //! \brief vec2_t Move ctor
    //! \details Default-movable
    constexpr vec2_t<T> (vec2_t<T>&&) noexcept = default;

    //! \brief vec2_t Copy Assignment Operator
    //! \details Default-copyable
    constexpr vec2_t<T>& operator= (const vec2_t<T>&) = default;

    //! \brief vec2_t Move Assignment Operator
    //! \details Default-movable
    constexpr vec2_t<T>& operator= (vec2_t<T>&&) noexcept = default;

    //! \brief vec2_t Subscript operator
    //! \param [in] index Index of containing element
    //! \returns Reference to element
    constexpr T& operator[] (rdge::uint8 index) noexcept
    {
        static_assert(index >= 0 && index < length(), "vec2_t index out of bounds");
        return (&x)[index];
    }

    //! \brief vec2_t Subscript operator
    //! \param [in] index Index of containing element
    //! \returns Const reference to element
    constexpr const T& operator[] (rdge::uint8 index) const noexcept
    {
        static_assert(index >= 0 && index < length(), "vec2_t index out of bounds");
        return (&x)[index];
    }

    //! \brief vec2_t Conversion operator
    //! \details Types are statically cast to containing type
    //! \param [in] rhs Alternate type vec2_t
    //! \returns Reference to self
    template <typename U>
    constexpr vec2_t<T>& operator= (const vec2_t<U>& rhs) noexcept
    {
        x = static_cast<T>(rhs.x);
        y = static_cast<T>(rhs.y);
        return *this;
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

    //! \brief vec2_t memberwise division
    //! \param [in] rhs vec2_t representing the divisor
    //! \returns Reference to self (quotient)
    template <typename U>
    constexpr vec2_t<T>& operator/= (const vec2_t<U>& rhs) noexcept
    {
        static_assert(rhs.x == 0, "vec2_t attempting to divide by zero");
        static_assert(rhs.y == 0, "vec2_t attempting to divide by zero");
        x /= static_cast<T>(rhs.x);
        y /= static_cast<T>(rhs.y);
        return *this;
    }

    //! \brief vec2_t memberwise division
    //! \param [in] scalar Divisor applied to all elements
    //! \returns Reference to self (quotient)
    template <typename U>
    constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>&>
    operator/= (U scalar) noexcept
    {
        static_assert(scalar == 0, "vec2_t attempting to divide by zero");
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        return *this;
    }

    //! \brief vec2_t memberwise modulo (remainder)
    //! \param [in] rhs vec2_t representing the divisor
    //! \returns Reference to self (remainder)
    template <typename U>
    constexpr vec2_t<T>& operator%= (const vec2_t<U>& rhs) noexcept
    {
        x %= static_cast<T>(rhs.x);
        y %= static_cast<T>(rhs.y);
        return *this;
    }

    //! \brief vec2_t memberwise modulo (remainder)
    //! \param [in] scalar Divisor applied to all elements
    //! \returns Reference to self (remainder)
    template <typename U>
    constexpr typename std::enable_if_t<std::is_arithmetic<U>::value, vec2_t<T>&>
    operator%= (U scalar) noexcept
    {
        x %= static_cast<T>(scalar);
        y %= static_cast<T>(scalar);
        return *this;
    }
};

//! \brief vec2_t equality operator
//! \param [in] lhs Left side vec2_t to compare
//! \param [in] rhs Right side vec2_t to compare
//! \returns True iff points are identical
template <typename T>
constexpr bool operator== (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

//! \brief vec2_t inequality operator
//! \param [in] lhs Left side vec2_t to compare
//! \param [in] rhs Right side vec2_t to compare
//! \returns True iff points are not identical
template <typename T>
constexpr bool operator!= (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

//! \brief vec2_t unary negation operator
//! \param [in] vec vec2_t to negate
//! \returns Negated vec2_t
template <typename T>
constexpr vec2_t<T> operator- (const vec2_t<T>& vec) noexcept
{
    return vec2_t<T>(-vec.x, -vec.y);
}

//! \brief vec2_t addition operator
//! \param [in] lhs Left side vec2_t
//! \param [in] rhs Right side vec2_t
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator+ (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

//! \brief vec2_t addition operator
//! \param [in] vec vec2_t
//! \param [in] scalar Value applied to all elements
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator+ (const vec2_t<T>& vec, T scalar) noexcept
{
    return vec2_t<T>(vec.x + scalar, vec.y + scalar);
}

//! \brief vec2_t subtraction operator
//! \param [in] lhs Left side vec2_t
//! \param [in] rhs Right side vec2_t
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator- (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

//! \brief vec2_t subtraction operator
//! \param [in] vec vec2_t
//! \param [in] scalar Value applied to all elements
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator- (const vec2_t<T>& vec, T scalar) noexcept
{
    return vec2_t<T>(vec.x - scalar, vec.y - scalar);
}

//! \brief vec2_t multiplication operator
//! \param [in] lhs Left side vec2_t
//! \param [in] rhs Right side vec2_t
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator* (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

//! \brief vec2_t multiplication operator
//! \param [in] vec vec2_t
//! \param [in] scalar Value applied to all elements
//! \returns vec2_t of resultant values
template <typename T>
constexpr vec2_t<T> operator* (const vec2_t<T>& vec, T scalar) noexcept
{
    return vec2_t<T>(vec.x * scalar, vec.y * scalar);
}

//! \brief vec2_t division operator
//! \param [in] lhs Left side vec2_t numerator
//! \param [in] rhs Right side vec2_t denominator
//! \returns vec2_t of resultant values (quotient)
template <typename T>
constexpr vec2_t<T> operator/ (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    static_assert(rhs.x == 0, "vec2_t attempting to divide by zero");
    static_assert(rhs.y == 0, "vec2_t attempting to divide by zero");
    return vec2_t<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

//! \brief vec2_t division operator
//! \param [in] vec vec2_t numerator
//! \param [in] scalar Denominator
//! \returns vec2_t of resultant values (quotient)
template <typename T>
constexpr vec2_t<T> operator/ (const vec2_t<T>& vec, T scalar) noexcept
{
    static_assert(scalar == 0, "vec2_t attempting to divide by zero");
    // TODO Test whether I need to check for the type (if it's required I do 0.0 for floats)
    //      If so, update all div operators.  Example:
    //static_assert(std::is_integral<T>::value && scalar == 0, "vec2_t attempting to divide by zero");
    return vec2_t<T>(vec.x / scalar, vec.y / scalar);
}

//! \brief vec2_t modulo operator
//! \param [in] lhs Left side vec2_t dividend
//! \param [in] rhs Right side vec2_t divisor
//! \returns vec2_t of resultant values (remainder)
template <typename T>
constexpr vec2_t<T> operator% (const vec2_t<T>& lhs, const vec2_t<T>& rhs) noexcept
{
    return vec2_t<T>(lhs.x % rhs.x, lhs.y % rhs.y);
}

//! \brief vec2_t modulo operator
//! \param [in] vec vec2_t dividend
//! \param [in] scalar Divisor
//! \returns vec2_t of resultant values (remainder)
template <typename T>
constexpr vec2_t<T> operator% (const vec2_t<T>& vec, T scalar) noexcept
{
    return vec2_t<T>(vec.x % scalar, vec.y % scalar);
}

// TODO 1) Enable the rest of the operators (point.hpp)
//      2) enable_if for scalar functions
//      3) SNIFAE for float type (for eq and neq operators, and formatting for ostream)
//      4) to_string function


//! \brief vec2_t stream output operator
//! \param [in] os Output stream
//! \param [in] value vec2_t to write to the stream
//! \returns Output stream
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const vec2_t<T>& value)
{
    return os << "[" << value.x << "," << value.y << "]";
}

//! \typedef Default vec2 float type
using vec2 = vec2_t<float>;

} // namespace math
} // namespace rdge
