#include <rdge/math/mat4.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/internal/logger_macros.hpp>

#include <cmath>
#include <sstream>
#include <iomanip>

namespace RDGE {
namespace Math {

mat4::mat4 (void)
{
    memset(elements, 0, sizeof(elements));
}

mat4::mat4 (float diagonal)
{
    memset(elements, 0, sizeof(elements));

    elements[0]  = diagonal;    // | I 0 0 0 |
    elements[5]  = diagonal;    // | 0 I 0 0 |
    elements[10] = diagonal;    // | 0 0 I 0 |
    elements[15] = diagonal;    // | 0 0 0 I |
}

mat4&
mat4::operator*= (const mat4& rhs)
{
    return multiply(rhs);
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
        WLOG("Unable to create inverse matrix.  Determinant is zero.");
        return mat4();
    }

    float inv_determinant = 1.0 / determinant;
    for (int i = 0; i < 16; i++)
    {
        result.elements[i] *= inv_determinant;
    }

    return result;
}

mat4&
mat4::multiply (const mat4& rhs)
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

/* static */ mat4
mat4::identity (void)
{
    return mat4(1.0f);
}

/* static */ mat4
mat4::orthographic (
                    float left,
                    float right,
                    float bottom,
                    float top,
                    float near,
                    float far
                   )
{
    mat4 result(1.0f);

    result.elements[0 + 0 * 4] = 2.0f / (right - left);
    result.elements[1 + 1 * 4] = 2.0f / (top - bottom);
    result.elements[2 + 2 * 4] = 2.0f / (near - far);

    result.elements[0 + 3 * 4] = (left + right) / (left - right);
    result.elements[1 + 3 * 4] = (bottom + top) / (bottom - top);
    result.elements[2 + 3 * 4] = (far + near) / (far - near);

    return result;
}

/* static */ mat4
mat4::perspective (
                   float field_of_view,
                   float aspect_ratio,
                   float near,
                   float far
                  )
{
    mat4 result(1.0f);

    float q = 1.0f / tan(to_radians(0.5f * field_of_view));
    float a = q / aspect_ratio;
    float b = (near + far) / (near - far);
    float c = (2.0f * near * far) / (near - far);

    result.elements[0 + 0 * 4] = a;         // | a  0  0  0 |
    result.elements[1 + 1 * 4] = q;         // | 0  q  0  0 |
    result.elements[2 + 2 * 4] = b;         // | 0  0  b  c |
    result.elements[3 + 2 * 4] = -1.0f;     // | 0  0  -1 1 |
    result.elements[2 + 3 * 4] = c;

    return result;
}

/* static */ mat4
mat4::translation (const vec3& translation)
{
    mat4 result(1.0f);

    result.elements[12] = translation.x;     // | 1 0 0 Vx |
    result.elements[13] = translation.y;     // | 0 1 0 Vy |
    result.elements[14] = translation.z;     // | 0 0 1 Vz |
                                             // | 0 0 0 1  |
    return result;
}

/* static */ mat4
mat4::rotation (float angle, const vec3& axis)
{
    mat4 result(1.0f);

    float theta = to_radians(angle);
    float c = cos(theta);
    float s = sin(theta);
    float arccos = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    // | xxC + c        xyC - zs        xzC + ys |
    // | yxC + zs       yyC + c         yzC - xs |
    // | zxC - ys       zyC + xs        zzC + c  |

    result.elements[0 + 0 * 4] = (x * arccos) + c;
    result.elements[1 + 0 * 4] = (y * x * arccos) + (z * s);
    result.elements[2 + 0 * 4] = (z * x * arccos) - (y * s);

    result.elements[0 + 1 * 4] = (x * y * arccos) - (z * s);
    result.elements[1 + 1 * 4] = (y * arccos) + c;
    result.elements[2 + 1 * 4] = (z * y * arccos) + (x * s);

    result.elements[0 + 2 * 4] = (x * z * arccos) + (y * s);
    result.elements[1 + 2 * 4] = (y * z * arccos) - (x * s);
    result.elements[2 + 2 * 4] = (z * arccos) + c;

    return result;
}

/* static */ mat4
mat4::scale (const vec3& scale)
{
    mat4 result(1.0f);

    result.elements[0 + 0 * 4] = scale.x;
    result.elements[1 + 1 * 4] = scale.y;
    result.elements[2 + 2 * 4] = scale.z;

    return result;
}

std::ostream& operator<< (std::ostream& os, const mat4& matrix)
{
    std::stringstream ss;
    ss << "[" << std::fixed << std::setprecision(5);

    for (RDGE::UInt32 i = 0; i < 4; ++i)
    {
        ss << "[" << matrix.columns[i].x << ","
           << matrix.columns[i].y << ","
           << matrix.columns[i].z << ","
           << matrix.columns[i].w << "]"
           << ((i == 3) ? "]" : ", ");
    }

    return os << ss.str();
}

} // namespace Math
} // namespace RDGE
