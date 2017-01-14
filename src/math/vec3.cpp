#include <rdge/math/vec3.hpp>

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

std::ostream& operator<< (std::ostream& os, const vec3& value)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "[" << value.x << ", " << value.y << ", " << value.z << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
