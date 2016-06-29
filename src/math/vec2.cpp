#include <rdge/math/vec2.hpp>

#include <sstream>
#include <iomanip>

namespace RDGE {
namespace Math {

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

std::ostream& operator<< (std::ostream& os, const vec2& vec)
{
    std::stringstream ss;
    ss << "[" << std::fixed << std::setprecision(5)
       << vec.x << ","
       << vec.y << "]";

    return os << ss.str();
}

} // namespace Math
} // namespace RDGE
