#include <rdge/math/mat3.hpp>
#include <rdge/math/intrinsics.hpp>

#include <SDL_assert.h>

#include <sstream>

namespace rdge {
namespace math {

mat3::mat3 (void)
{
    memset(this->elements, 0, sizeof(this->elements));
}

vec3&
mat3::operator[] (uint8 index) noexcept
{
    SDL_assert(index < 3);
    return this->columns[index];
}

const vec3&
mat3::operator[] (uint8 index) const noexcept
{
    SDL_assert(index < 3);
    return this->columns[index];
}

float
mat3::determinant (void) const noexcept
{
    return (elements[0] * (elements[4] * elements[8] - elements[7] * elements[5])) -
           (elements[3] * (elements[1] * elements[8] - elements[7] * elements[2])) +
           (elements[6] * (elements[1] * elements[5] - elements[4] * elements[2]));
}

vec3
mat3::solve (const vec3& b) const noexcept
{
    float det = determinant();
    if (det != 0.f)
    {
        det = 1.f / det;
    }

    mat3 inv;
    inv[0].x = elements[4] * elements[8] - elements[5] * elements[7];
    inv[0].y = -elements[1] * elements[8] + elements[7] * elements[2];
    inv[0].z = elements[1] * elements[5] - elements[4] * elements[2];

    inv[1].x = -elements[3] * elements[8] + elements[6] * elements[5];
    inv[1].y = elements[0] * elements[8] - elements[6] * elements[2];
    inv[1].z = -elements[0] * elements[5] + elements[3] * elements[2];

    inv[2].x = elements[3] * elements[7] - elements[6] * elements[4];
    inv[2].y = -elements[0] * elements[7] + elements[6] * elements[1];
    inv[2].z = elements[0] * elements[4] - elements[3] * elements[1];

    vec3 result;
    result.x = math::dot(inv[0], b) * det;
    result.y = math::dot(inv[1], b) * det;
    result.z = math::dot(inv[2], b) * det;

    return result;
}

vec2
mat3::solve (const vec2& b) const noexcept
{
    float det = (elements[0] * elements[4]) - (elements[1] * elements[3]);
    if (det != 0.f)
    {
        det = 1.f / det;
    }

    vec2 result;
    result.x = (elements[4] * b.x - elements[1] * b.y) * det;
    result.y = (elements[0] * b.y - elements[3] * b.x) * det;

    return result;
}

/* static */ mat3
mat3::identity (void)
{
    mat3 result;
    result[0][0] = 1.f;    // | I 0 0 |
    result[1][1] = 1.f;    // | 0 I 0 |
    result[2][2] = 1.f;    // | 0 0 I |

    return result;
}

std::ostream& operator<< (std::ostream& os, const mat3& matrix)
{
    std::ostringstream ss;
    ss << "[" << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
