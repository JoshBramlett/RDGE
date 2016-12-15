//! \headerfile <rdge/math/vec3.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/functions.hpp>

#include <ostream>
#include <sstream>
#include <iomanip>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct vec3
//! \brief Three dimensional floating point vector
struct vec3
{
    float x;
    float y;
    float z;

    vec3 (void);
    //constexpr vec3 (void)
        //: x(0.0f)
        //, y(0.0f)
        //, z(0.0f)
    //{ }

    explicit vec3 (float x, float y, float z);
    //constexpr vec3 (float x, float y, float z)
        //: x(x)
        //, y(y)
        //, z(z)
    //{ }

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
    std::ostringstream ss;
    ss << "[" << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << ","
       << vec.z << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
