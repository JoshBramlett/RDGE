//! \headerfile <rdge/math/functions.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/23/2016
//! \see http://graphics.stanford.edu/~seander/bithacks.html

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/compiler.hpp>

//#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <limits>
#include <type_traits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//!@{ \brief Math constants
constexpr float PI = 3.14159265358979f;
constexpr float TWO_PI = PI * 2.f;
//!@}

//! \brief Floating point equality
//! \details Uses machine epsilon scaled to the magnitue of the values used.
//! \param [in] x First value to compare
//! \param [in] y Second value to compare
//! \returns True if equal, false if not.
//! \note Matching NaN and Infinity types compare equal
//! \see http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool>
fp_eq (T x, T y)
{
    return (std::isnan(x) && std::isnan(y)) ||
           (std::isinf(x) && std::isinf(y)) ||
           (std::abs(x - y) * 2) < std::numeric_limits<T>::epsilon() ||
           std::abs(x-y) < std::numeric_limits<T>::min();
}

//! \brief Zero check floating point specialization
//! \details Classification is defined as 0.0 or -0.0.
//! \warning This does not include an epsilon check
//! \param [in] val Value to check
//! \returns True iff value is zero
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool>
is_zero (T val)
{
    return (RDGE_FPCLASSIFY(val) == FP_ZERO);
}

//! \brief Zero check integral specialization
//! \param [in] val Value to check
//! \returns True iff value is zero
template <typename T>
constexpr typename std::enable_if_t<std::is_integral<T>::value, bool>
is_zero (T val)
{
    return (val == 0);
}

//! \brief Clamp value between upper and lower bounds
//! \details If the value is greater than or less then the boundaries, the
//!          boundary values are retured.
//! \param [in] val Value to check
//! \param [in] lbound Lower bound
//! \param [in] ubound Upper bound
//! \returns Clamped value
template <typename T>
constexpr typename std::enable_if_t<std::is_arithmetic<T>::value, T>
clamp (T val, T lbound, T ubound)
{
    return std::max(lbound, std::min(val, ubound));
}

//! \brief Get the signage of a given value
//! \details The integer returned will be 1, 0, or -1 representing the
//!          sign of the provided value
//! \param [in] val Value to check
//! \returns Integer representing the sign
template <typename T>
constexpr typename std::enable_if_t<std::is_arithmetic<T>::value, int32>
sign (T val)
{
    return (T(0) < val) - (val < T(0));
}

//! \brief Get the absolute value
//! \param [in] val Value to compute
//! \returns Positive value
template <typename T>
constexpr typename std::enable_if_t<std::is_arithmetic<T>::value, T>
abs (T val)
{
    return ((val) < 0 ? -(val) : (val));
}

//! \brief Square the value
//! \param [in] val Value to square
//! \returns Value squared
template <typename T>
constexpr typename std::enable_if_t<std::is_arithmetic<T>::value, T>
square (T val)
{
    return (val * val);
}

//! \brief Get the position of the least significant bit
//! \param [in] val Value to check
//! \returns One plus the lsb index, or zero if the parameter is zero
constexpr int32 lsb (int64 val)
{
    return RDGE_LSB(val);
}

//! \brief Check if value is a power of two
//! \param [in] val Value to check
//! \returns True if value is a power of two, false otherwise
constexpr bool is_pot (uint64 val)
{
    return (val != 0) && ((val & (val - 1)) == 0);
}

//! \brief Convert degrees to radians
//! \param [in] degrees Degrees to convert
//! \returns Value in radians
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, T>
to_radians (T degrees)
{
    return degrees * 0.0174532925199432957;
}

//! \brief Convert radians to degrees
//! \param [in] radians Radians to convert
//! \returns Value in degrees
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, T>
to_degrees (T radians)
{
    return radians * 57.295779513082320876;
}

} // namespace math
} // namespace rdge
