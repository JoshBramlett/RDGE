//! \headerfile <rdge/math/vec2.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>
#include <iomanip>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

struct vec2
{
    float x;
    float y;

    vec2 (void);

    explicit vec2 (float x, float y);

    vec2& add (const vec2& rhs);

    vec2& subtract (const vec2& rhs);

    vec2& multiply (const vec2& rhs);

    vec2& divide (const vec2& rhs);

    vec2& operator+= (const vec2& rhs);

    vec2& operator-= (const vec2& rhs);

    vec2& operator*= (const vec2& rhs);

    vec2& operator/= (const vec2& rhs);
};

inline bool operator== (const vec2& lhs, const vec2& rhs)
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y);
}

inline bool operator!= (const vec2& lhs, const vec2& rhs)
{
    return !fp_eq(lhs.x, rhs.x) || !fp_eq(lhs.y, rhs.y);
}

inline vec2 operator+ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline vec2 operator- (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline vec2 operator* (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2 operator/ (const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

inline std::ostream& operator<< (std::ostream& os, const vec2& vec)
{
    auto f = os.flags();
    auto p = os.precision();

    os << "vec2: ["
       << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << "]";

    os.precision(p);
    os.flags(f);

    return os;
}

} // namespace Math
} // namespace RDGE
