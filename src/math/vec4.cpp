#include <rdge/math/vec4.hpp>

#include <sstream>
#include <iomanip>

namespace RDGE {
namespace Math {

vec4&
vec4::add (const vec4& value)
{
    x += value.x;
    y += value.y;
    z += value.z;
    w += value.w;

    return *this;
}

vec4&
vec4::subtract (const vec4& value)
{
    x -= value.x;
    y -= value.y;
    z -= value.z;
    w -= value.w;

    return *this;
}

vec4&
vec4::multiply (const vec4& value)
{
    x *= value.x;
    y *= value.y;
    z *= value.z;
    w *= value.w;

    return *this;
}

vec4&
vec4::divide (const vec4& value)
{
    x /= value.x;
    y /= value.y;
    z /= value.z;
    w /= value.w;

    return *this;
}

vec4&
vec4::operator+= (const vec4& rhs)
{
    return add(rhs);
}

vec4&
vec4::operator-= (const vec4& rhs)
{
    return subtract(rhs);
}

vec4&
vec4::operator*= (const vec4& rhs)
{
    return multiply(rhs);
}

vec4&
vec4::operator/= (const vec4& rhs)
{
    return divide(rhs);
}

std::ostream& operator<< (std::ostream& os, const vec4& vec)
{
    std::ostringstream ss;
    ss << "[" << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << ","
       << vec.z << ","
       << vec.w << "]";

    return os << ss.str();
}

} // namespace Math
} // namespace RDGE
