#include <rdge/math/vec4.hpp>

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

} // namespace Math
} // namespace RDGE
