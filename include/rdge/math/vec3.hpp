//! \headerfile <rdge/math/vec3.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/util/fp.hpp>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

struct vec3
{
    float x;
    float y;
    float z;

    vec3 (void);

    explicit vec3 (float x, float y, float z);

    vec3& add (const vec3& rhs);

    vec3& subtract (const vec3& rhs);

    vec3& multiply (const vec3& rhs);

    vec3& divide (const vec3& rhs);

    vec3& operator+= (const vec3& rhs);

    vec3& operator-= (const vec3& rhs);

    vec3& operator*= (const vec3& rhs);

    vec3& operator/= (const vec3& rhs);
};

inline bool operator== (const vec3& lhs, const vec3& rhs)
{
    return fp_eq(lhs.x, rhs.x) && fp_eq(lhs.y, rhs.y) && fp_eq(lhs.z, rhs.z);
}

inline bool operator!= (const vec3& lhs, const vec3& rhs)
{
    return !fp_eq(lhs.x, rhs.x) || !fp_eq(lhs.y, rhs.y) || !fp_eq(lhs.z, rhs.z);
}

inline vec3 operator+ (const vec3& lhs, const vec3& rhs)
{
    return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline vec3 operator- (const vec3& lhs, const vec3& rhs)
{
    return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline vec3 operator* (const vec3& lhs, const vec3& rhs)
{
    return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

inline vec3 operator/ (const vec3& lhs, const vec3& rhs)
{
    return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

inline std::ostream& operator<< (std::ostream& os, const vec3& vec)
{
    auto f = os.flags();
    auto p = os.precision();

    os << "vec3: ["
       << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << ","
       << vec.z << "]";

    os.precision(p);
    os.flags(f);

    return os;
}

} // namespace Math
} // namespace RDGE
