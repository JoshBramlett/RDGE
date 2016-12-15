#include <rdge/math/vec3.hpp>

using namespace rdge::math;

vec3::vec3 (void)
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
{ }

vec3::vec3 (float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{ }

vec3&
vec3::add (const vec3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
}

vec3&
vec3::subtract (const vec3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;

    return *this;
}

vec3&
vec3::multiply (const vec3& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;

    return *this;
}

vec3&
vec3::divide (const vec3& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;

    return *this;
}

vec3&
vec3::operator+= (const vec3& rhs)
{
    return add(rhs);
}

vec3&
vec3::operator-= (const vec3& rhs)
{
    return subtract(rhs);
}

vec3&
vec3::operator*= (const vec3& rhs)
{
    return multiply(rhs);
}

vec3&
vec3::operator/= (const vec3& rhs)
{
    return divide(rhs);
}
