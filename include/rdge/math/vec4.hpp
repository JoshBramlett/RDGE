//! \headerfile <rdge/math/vec4.hpp>
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

struct vec4
{
    float x;
    float y;
    float z;
    float w;

    vec4 (void);

    explicit vec4 (float x, float y, float z, float w);

    vec4& add (const vec4& rhs);

    vec4& subtract (const vec4& rhs);

    vec4& multiply (const vec4& rhs);

    vec4& divide (const vec4& rhs);

    vec4& operator+= (const vec4& rhs);

    vec4& operator-= (const vec4& rhs);

    vec4& operator*= (const vec4& rhs);

    vec4& operator/= (const vec4& rhs);
};

inline bool operator== (const vec4& lhs, const vec4& rhs)
{
    return fp_eq(lhs.x, rhs.x) &&
           fp_eq(lhs.y, rhs.y) &&
           fp_eq(lhs.z, rhs.z) &&
           fp_eq(lhs.w, rhs.w);
}

inline bool operator!= (const vec4& lhs, const vec4& rhs)
{
    return !fp_eq(lhs.x, rhs.x) ||
           !fp_eq(lhs.y, rhs.y) ||
           !fp_eq(lhs.z, rhs.z) ||
           !fp_eq(lhs.w, rhs.w);
}

inline vec4 operator+ (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

inline vec4 operator- (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

inline vec4 operator* (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

inline vec4 operator/ (const vec4& lhs, const vec4& rhs)
{
    return vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

inline std::ostream& operator<< (std::ostream& os, const vec4& vec)
{
    auto f = os.flags();
    auto p = os.precision();

    os << "vec4: ["
       << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << ","
       << vec.z << ","
       << vec.w << "]";

    os.precision(p);
    os.flags(f);

    return os;
}

} // namespace Math
} // namespace RDGE
