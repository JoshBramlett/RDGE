#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>

#include <SDL_assert.h>

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
    SDL_assert(index < 3);
    return (&this->x)[index];
}

const float&
vec3::operator[] (uint8 index) const noexcept
{
    SDL_assert(index < 3);
    return (&this->x)[index];
}

vec3&
vec3::apply_transform (const mat4& mat) noexcept
{
    float xx = (mat[0].x * this->x) + (mat[1].x * this->y) + (mat[2].x * this->z) + mat[3].x;
    float xy = (mat[0].y * this->x) + (mat[1].y * this->y) + (mat[2].y * this->z) + mat[3].y;
    float xz = (mat[0].z * this->x) + (mat[1].z * this->y) + (mat[2].z * this->z) + mat[3].z;

    this->x = xx;
    this->y = xy;
    this->z = xz;
    return *this;
}

vec3
vec3::apply_transform (const mat4& mat) const noexcept
{
    vec3 result = *this;
    return result.apply_transform(mat);
}

std::ostream& operator<< (std::ostream& os, const vec3& value)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "[" << value.x << ", " << value.y << ", " << value.z << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
