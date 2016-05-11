//! \headerfile <rdge/math/functions.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/23/2016
//! \bug

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

//! \brief Floating point comparison
//! \details Uses the machine epsilon scaled to the magnitue of the values
//!          used, multiplied by the desired precision in ULPs (units in
//!          last place).  NaN and Infinity checking is performed.  Template
//!          ensures a non-integer numeric type.
//! \param [in] x First value to compare
//! \param [in] y Second value to compare
//! \param [in] ulp Precision to check for
//! \returns True if equal, false if not.
//! \see http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <typename T>
constexpr typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
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
constexpr T clamp (T val, T lbound, T ubound) noexcept
{
    return std::max(lbound, std::min(val, ubound));
}

//! \brief Get the signage of a given value
//! \details The integer returned will be 1, 0, or -1 representing the
//!          sign of the provided value
//! \param [in] val Value to check
//! \returns Integer representing the sign
template <typename T>
constexpr int sign (T val) noexcept
{
    return (T(0) < val) - (val < T(0));
}

//! \brief Convert degrees to radians
//! \param [in] degrees Degrees to convert
//! \returns Value in radians
constexpr float to_radians (float degrees) noexcept
{
    return static_cast<float>(degrees * (M_PI / 180.0f));
}

//! \brief Convert radians to degrees
//! \param [in] radians Radians to convert
//! \returns Value in degrees
constexpr float to_degrees (float radians) noexcept
{
    return static_cast<float>(radians * (180.0f / M_PI));
}

} // namespace Math
} // namespace RDGE
