//! \headerfile <rdge/math/functions.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/23/2016
//! \see http://graphics.stanford.edu/~seander/bithacks.html

#pragma once

//#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <limits>
#include <type_traits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \brief Floating point equality
//! \details Uses machine epsilon scaled to the magnitue of the values used.
//! \param [in] x First value to compare
//! \param [in] y Second value to compare
//! \returns True if equal, false if not.
//! \note Matching NaN and Infinity types compare equal
//! \see http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool>
fp_eq (T x, T y) noexcept
{
    return (std::isnan(x) && std::isnan(y)) ||
           (std::isinf(x) && std::isinf(y)) ||
           (std::abs(x - y) * 2) < std::numeric_limits<T>::epsilon() ||
           std::abs(x-y) < std::numeric_limits<T>::min();
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
clamp (T val, T lbound, T ubound) noexcept
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
sign (T val) noexcept
{
    return (T(0) < val) - (val < T(0));
}

//! \brief Get the position of the least significant bit
//! \param [in] val Value to check
//! \returns One plus the lsb index, or zero if the parameter is zero
constexpr int32 lsb (int64 val) noexcept
{
    // TODO Add Windows support
    return __builtin_ffsll(val);
}

//! \brief Check if value is a power of two
//! \param [in] val Value to check
//! \returns True if value is a power of two, false otherwise
constexpr bool is_pot (uint64 val) noexcept
{
    return (val != 0) && ((val & (val - 1)) == 0);
}

//! \brief Convert degrees to radians
//! \param [in] degrees Degrees to convert
//! \returns Value in radians
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, T>
to_radians (T degrees) noexcept
{
    return static_cast<T>(degrees * (M_PI / 180.f));
}

//! \brief Convert radians to degrees
//! \param [in] radians Radians to convert
//! \returns Value in degrees
template <typename T>
constexpr typename std::enable_if_t<std::is_floating_point<T>::value, T>
to_degrees (T radians) noexcept
{
    return static_cast<T>(radians * (180.f / M_PI));
}

} // namespace math
} // namespace rdge
