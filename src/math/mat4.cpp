#include <rdge/math/mat4.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <cmath>
#include <sstream>
#include <iomanip>

namespace rdge {
namespace math {

mat4::mat4 (void)
{
    memset(this->elements, 0, sizeof(this->elements));
}

vec4&
mat4::operator[] (uint8 index) noexcept
{
    SDL_assert(index < 4);
    return this->columns[index];
}

const vec4&
mat4::operator[] (uint8 index) const noexcept
{
    SDL_assert(index < 4);
    return this->columns[index];
}

mat4&
mat4::operator*= (const mat4& rhs)
{
    float data[16];
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            float sum = 0.0f;
            for (int e = 0; e < 4; e++)
            {
                sum += elements[x + e * 4] * rhs.elements[e + y * 4];
            }

            data[x + y * 4] = sum;
        }
    }

    memcpy(elements, data, 4 * 4 * sizeof(float));
    return *this;
}

mat4
mat4::inverse (void)
{
    mat4 result;

    result.elements[0]  = elements[5]  * elements[10] * elements[15] -
                          elements[5]  * elements[11] * elements[14] -
                          elements[9]  * elements[6]  * elements[15] +
                          elements[9]  * elements[7]  * elements[14] +
                          elements[13] * elements[6]  * elements[11] -
                          elements[13] * elements[7]  * elements[10];

    result.elements[4]  = elements[12] * elements[10] * elements[7]  -
                          elements[8]  * elements[14] * elements[7]  -
                          elements[12] * elements[6]  * elements[11] +
                          elements[4]  * elements[14] * elements[11] +
                          elements[8]  * elements[6]  * elements[15] -
                          elements[4]  * elements[10] * elements[15];

    result.elements[8]  = elements[4]  * elements[9]  * elements[15] -
                          elements[4]  * elements[11] * elements[13] -
                          elements[8]  * elements[5]  * elements[15] +
                          elements[8]  * elements[7]  * elements[13] +
                          elements[12] * elements[5]  * elements[11] -
                          elements[12] * elements[7]  * elements[9];

    result.elements[12] = elements[12] * elements[9]  * elements[6]  -
                          elements[8]  * elements[13] * elements[6]  -
                          elements[12] * elements[5]  * elements[10] +
                          elements[4]  * elements[13] * elements[10] +
                          elements[8]  * elements[5]  * elements[14] -
                          elements[4]  * elements[9]  * elements[14];

    result.elements[1]  = elements[13] * elements[10] * elements[3]  -
                          elements[9]  * elements[14] * elements[3]  -
                          elements[13] * elements[2]  * elements[11] +
                          elements[1]  * elements[14] * elements[11] +
                          elements[9]  * elements[2]  * elements[15] -
                          elements[1]  * elements[10] * elements[15];

    result.elements[5]  = elements[0]  * elements[10] * elements[15] -
                          elements[0]  * elements[11] * elements[14] -
                          elements[8]  * elements[2]  * elements[15] +
                          elements[8]  * elements[3]  * elements[14] +
                          elements[12] * elements[2]  * elements[11] -
                          elements[12] * elements[3]  * elements[10];

    result.elements[9]  = elements[12] * elements[3]  * elements[9]  -
                          elements[8]  * elements[13] * elements[3]  -
                          elements[12] * elements[1]  * elements[11] +
                          elements[0]  * elements[13] * elements[11] +
                          elements[8]  * elements[1]  * elements[15] -
                          elements[0]  * elements[9]  * elements[15];

    result.elements[13] = elements[0]  * elements[9]  * elements[14] -
                          elements[0]  * elements[10] * elements[13] -
                          elements[8]  * elements[1]  * elements[14] +
                          elements[8]  * elements[2]  * elements[13] +
                          elements[12] * elements[1]  * elements[10] -
                          elements[12] * elements[2]  * elements[9];

    result.elements[2]  = elements[1]  * elements[6]  * elements[15] -
                          elements[1]  * elements[7]  * elements[14] -
                          elements[5]  * elements[2]  * elements[15] +
                          elements[5]  * elements[3]  * elements[14] +
                          elements[13] * elements[2]  * elements[7]  -
                          elements[13] * elements[3]  * elements[6];

    result.elements[6]  = elements[12] * elements[6]  * elements[3]  -
                          elements[4]  * elements[14] * elements[3]  -
                          elements[12] * elements[2]  * elements[7]  +
                          elements[0]  * elements[14] * elements[7]  +
                          elements[4]  * elements[2]  * elements[15] -
                          elements[0]  * elements[6]  * elements[15];

    result.elements[10] = elements[0]  * elements[5]  * elements[15] -
                          elements[0]  * elements[7]  * elements[13] -
                          elements[4]  * elements[1]  * elements[15] +
                          elements[4]  * elements[3]  * elements[13] +
                          elements[12] * elements[1]  * elements[7]  -
                          elements[12] * elements[3]  * elements[5];

    result.elements[14] = elements[12] * elements[5]  * elements[2]  -
                          elements[4]  * elements[13] * elements[2]  -
                          elements[12] * elements[1]  * elements[6]  +
                          elements[0]  * elements[13] * elements[6]  +
                          elements[4]  * elements[1]  * elements[14] -
                          elements[0]  * elements[5]  * elements[14];

    result.elements[3]  = elements[9]  * elements[6]  * elements[3]  -
                          elements[5]  * elements[10] * elements[3]  -
                          elements[9]  * elements[2]  * elements[7]  +
                          elements[1]  * elements[10] * elements[7]  +
                          elements[5]  * elements[2]  * elements[11] -
                          elements[1]  * elements[6]  * elements[11];

    result.elements[7] =  elements[0]  * elements[6]  * elements[11] -
                          elements[0]  * elements[7]  * elements[10] -
                          elements[4]  * elements[2]  * elements[11] +
                          elements[4]  * elements[3]  * elements[10] +
                          elements[8]  * elements[2]  * elements[7]  -
                          elements[8]  * elements[3]  * elements[6];

    result.elements[11] = elements[8]  * elements[5]  * elements[3]  -
                          elements[4]  * elements[9]  * elements[3]  -
                          elements[8]  * elements[1]  * elements[7]  +
                          elements[0]  * elements[9]  * elements[7]  +
                          elements[4]  * elements[1]  * elements[11] -
                          elements[0]  * elements[5]  * elements[11];

    result.elements[15] = elements[0]  * elements[5]  * elements[10] -
                          elements[0]  * elements[6]  * elements[9]  -
                          elements[4]  * elements[1]  * elements[10] +
                          elements[4]  * elements[2]  * elements[9]  +
                          elements[8]  * elements[1]  * elements[6]  -
                          elements[8]  * elements[2]  * elements[5];

    float determinant = elements[0] * result.elements[0] +
                        elements[1] * result.elements[4] +
                        elements[2] * result.elements[8] +
                        elements[3] * result.elements[12];
    if (UNLIKELY(fp_eq(determinant, 0.0f)))
    {
        WLOG() << "Unable to create inverse matrix.  Determinant is zero.";
        return mat4();
    }

    float inv_determinant = 1.0 / determinant;
    for (int i = 0; i < 16; i++)
    {
        result.elements[i] *= inv_determinant;
    }

    return result;
}

/* static */ mat4
mat4::identity (void)
{
    mat4 result;
    result[0][0] = 1.f;    // | I 0 0 0 |
    result[1][1] = 1.f;    // | 0 I 0 0 |
    result[2][2] = 1.f;    // | 0 0 I 0 |
    result[3][3] = 1.f;    // | 0 0 0 I |

    return result;
}

/* static */ mat4
mat4::orthographic (float left, float right, float bottom, float top, float near, float far)
{
    auto result = mat4::identity();
    result[0][0] = 2.f / (right - left);
    result[1][1] = 2.f / (top - bottom);
    result[2][2] = -2.f / (far - near);
    result[3][0] = -((right + left) / (right - left));
    result[3][1] = -((top + bottom) / (top - bottom));
    result[3][2] = -((far + near) / (far - near));

    return result;
}

/* static */ mat4
mat4::perspective (float field_of_view, float aspect_ratio, float near, float far)
{
    auto result = mat4();
    float tan_half_fov = tan(to_radians(0.5f * field_of_view));
    result[0][0] = 1.f / (tan_half_fov * aspect_ratio);  // | a  0  0  0 |
    result[1][1] = 1.f / tan_half_fov;                   // | 0  q  0  0 |
    result[2][2] = -(far + near) / (far - near);         // | 0  0  b  c |
    result[2][3] = -1.f;                                 // | 0  0  -1 0 |
    result[3][2] = -(2.f * far * near) / (far - near);

    return result;
}

/* static */ mat4
mat4::translation (const vec3& translation)
{
    auto result = mat4::identity();
    result[3][0] = translation.x;     // | 1 0 0 Vx |
    result[3][1] = translation.y;     // | 0 1 0 Vy |
    result[3][2] = translation.z;     // | 0 0 1 Vz |
                                      // | 0 0 0 1  |
    return result;
}

/* static */ mat4
mat4::rotation (float angle, const vec3& axis)
{
    float theta = to_radians(angle);
    float c = cos(theta);
    float s = sin(theta);
    float arccos = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    // | xxC + c     xyC - zs    xzC + ys |
    // | yxC + zs    yyC + c     yzC - xs |
    // | zxC - ys    zyC + xs    zzC + c  |

    auto result = mat4::identity();
    result[0][0] = (x * x * arccos) + c;
    result[0][1] = (y * x * arccos) + (z * s);
    result[0][2] = (z * x * arccos) - (y * s);

    result[1][0] = (x * y * arccos) - (z * s);
    result[1][1] = (y * y * arccos) + c;
    result[1][2] = (z * y * arccos) + (x * s);

    result[2][0] = (x * z * arccos) + (y * s);
    result[2][1] = (y * z * arccos) - (x * s);
    result[2][2] = (z * z * arccos) + c;

    return result;
}

/* static */ mat4
mat4::scale (const vec3& scale)
{
    auto result = mat4::identity();
    result[0][0] = scale.x;
    result[1][1] = scale.y;
    result[2][2] = scale.z;

    return result;
}

/* static */ mat4
mat4::look_at (const vec3& eye, const vec3& center, const vec3& up)
{
    vec3 f = (center - eye).normalize(); // forward vector
    vec3 s = f.cross(up).normalize();    // side vector
    vec3 u = s.cross(f);                 // real up vector

    auto result = mat4::identity();
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;             // | s.x   u.x   -f.x   -s.dot(eye) |
    result[1][1] = u.y;             // | s.y   u.y   -f.y   -u.dot(eye) |
    result[2][1] = u.z;             // | s.z   u.z   -f.z    f.dot(eye) |
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][0] = -s.dot(eye);
    result[3][1] = -u.dot(eye);
    result[3][2] = f.dot(eye);

    // dot products above are the inverted eye coordinates, which alternatively
    // are set by multiplying an inverted eye transformation matrix
    //mat4 inverted_eye = mat4::translation(-eye);
    //return result * inverted_eye;

    return result;
}

std::ostream& operator<< (std::ostream& os, const mat4& matrix)
{
    std::ostringstream ss;
    ss << "[" << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << ", " << matrix[3] << "]";

    return os << ss.str();
}

} // namespace math
} // namespace rdge
