#include <rdge/math/vec4.hpp>

namespace RDGE {
namespace Math {

vec4::vec4 (void)
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , w(0.0f)
{ }

vec4::vec4 (float x, float y, float z, float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{ }

vec4&
vec4::add (const vec4& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;

    return *this;
}

vec4&
vec4::subtract (const vec4& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;

    return *this;
}

vec4&
vec4::multiply (const vec4& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;

    return *this;
}

vec4&
vec4::divide (const vec4& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;

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
