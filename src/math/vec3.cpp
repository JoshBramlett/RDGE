#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/debug/assert.hpp>

#include <sstream>
#include <iomanip>

namespace rdge {
namespace math {

const vec3 vec3::ZERO (0.f, 0.f, 0.f);
const vec3 vec3::X (1.f, 0.f, 0.f);
const vec3 vec3::Y (0.f, 1.f, 0.f);
const vec3 vec3::Z (0.f, 0.f, 1.f);

float&
vec3::operator[] (uint8 index) noexcept
{
    RDGE_ASSERT(index < 3);
    return (&this->x)[index];
}

float
vec3::operator[] (uint8 index) const noexcept
{
    RDGE_ASSERT(index < 3);
    return (&this->x)[index];
}

vec3&
vec3::transform (const mat4& xf) noexcept
{
    float xx = (xf[0].x * x) + (xf[1].x * y) + (xf[2].x * z) + xf[3].x;
    float xy = (xf[0].y * x) + (xf[1].y * y) + (xf[2].y * z) + xf[3].y;
    float xz = (xf[0].z * x) + (xf[1].z * y) + (xf[2].z * z) + xf[3].z;

    x = xx;
    y = xy;
    z = xz;
    return *this;
}

vec3
vec3::transform (const mat4& xf) const noexcept
{
    vec3 result = *this;
    return result.transform(xf);
}

std::ostream& operator<< (std::ostream& os, const vec3& value)
{
    return os << rdge::to_string(value);
}

} // namespace math

std::string
to_string (const math::vec3& value)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "[" << value.x << ", " << value.y << ", " << value.z << "]";

    return ss.str();
}

} // namespace rdge
