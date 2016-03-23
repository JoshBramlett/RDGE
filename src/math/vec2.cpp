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
vec2::operator/= (const vec2& rhs)
{
    return divide(rhs);
}

} // namespace Math
} // namespace RDGE
