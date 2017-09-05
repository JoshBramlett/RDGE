#include <rdge/math/mat2.hpp>
#include <rdge/math/intrinsics.hpp>

#include <SDL_assert.h>

#include <sstream>

namespace rdge {
namespace math {

mat2::mat2 (void)
{
    memset(this->elements, 0, sizeof(this->elements));
}

vec2&
mat2::operator[] (uint8 index) noexcept
{
    SDL_assert(index < 2);
    return this->columns[index];
}

const vec2&
mat2::operator[] (uint8 index) const noexcept
{
    SDL_assert(index < 2);
    return this->columns[index];
}

float
mat2::determinant (void) const noexcept
{
    return (elements[0] * elements[3]) - (elements[1] * elements[2]);
}

vec2
mat2::solve (const vec2& b) const noexcept
{
    float det = determinant();
    if (det != 0.f)
    {
        det = 1.f / det;
    }

    vec2 result;
    result.x = (elements[3] * b.x - elements[1] * b.y) * det;
    result.y = (elements[0] * b.y - elements[2] * b.x) * det;

    return result;
}

/* static */ mat2
mat2::identity (void)
{
    mat2 result;
    result[0][0] = 1.f;    // | I 0 |
    result[1][1] = 1.f;    // | 0 I |

    return result;
}

std::ostream& operator<< (std::ostream& os, const mat2& matrix)
{
    std::ostringstream ss;
    ss << "[" << matrix[0] << ", " << matrix[1] << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
