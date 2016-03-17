//! \headerfile <rdge/util/fp.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/24/2015
//! \bug

#pragma once

#include <cmath>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Util {

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

} // namespace Util
} // namespace RDGE
