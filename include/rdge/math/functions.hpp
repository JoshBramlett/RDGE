//! \headerfile <rdge/math/functions.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/23/2016
//! \bug

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

template <typename T> constexpr T Epsilon;
template <> constexpr double Epsilon<double>           = 0.0000001;
template <> constexpr long double Epsilon<long double> = 0.0000001;
template <> constexpr float Epsilon<float>             = 0.0000001f;

template <typename T>
constexpr bool fp_eq (T p1, T p2)
{
    return (std::isnan(p1) && std::isnan(p2)) ||
           (std::isinf(p1) && std::isinf(p2)) ||
           std::fabs(p1 - p2) <= Epsilon<T>;
}

template <typename T>
constexpr int sign (T val)
{
    return (T(0) < val) - (val < T(0));
}

constexpr double to_radians (double degrees)
{
    return static_cast<double>(degrees * (M_PI / 180.0));
}

constexpr double to_degrees (double radians)
{
    return static_cast<double>(radians * (180.0 / M_PI));
}

} // namespace Math
} // namespace RDGE
