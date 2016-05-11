#include <rdge/math/vec2.hpp>

namespace RDGE {
namespace Math {

vec2::vec2 (void)
    : x(0.0f)
    , y(0.0f)
{ }

vec2::vec2 (float x, float y)
    : x(x)
    , y(y)
{ }

vec2&
vec2::add (const vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;

    return *this;
}

vec2&
vec2::subtract (const vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;

    return *this;
}

vec2&
vec2::multiply (const vec2& rhs)
{
    x *= rhs.x;
    y *= rhs.y;

    return *this;
}

vec2&
vec2::divide (const vec2& rhs)
{
    x /= rhs.x;
    y /= rhs.y;

    return *this;
}

vec2&
vec2::scale (float value)
{
    x *= value;
    y *= value;

    return *this;
}

vec2&
vec2::operator+= (const vec2& rhs)
{
    return add(rhs);
}

vec2&
vec2::operator-= (const vec2& rhs)
{
    return subtract(rhs);
}

vec2&
vec2::operator*= (const vec2& rhs)
{
    return multiply(rhs);
}

vec2&
vec2::operator*= (float rhs)
{
    return scale(rhs);
}

vec2&
vec2::operator/= (const vec2& rhs)
{
    return divide(rhs);
}

} // namespace Math
} // namespace RDGE
