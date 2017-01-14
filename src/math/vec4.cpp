#include <rdge/math/vec4.hpp>

#include <SDL_assert.h>

#include <sstream>
#include <iomanip>

namespace rdge {
namespace math {

float&
vec4::operator[] (uint8 index) noexcept
{
    SDL_assert(index < 4);
    return (&this->x)[index];
}

const float&
vec4::operator[] (uint8 index) const noexcept
{
    SDL_assert(index < 4);
    return (&this->x)[index];
}

std::ostream& operator<< (std::ostream& os, const vec4& vec)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
