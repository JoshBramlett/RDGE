#include <rdge/math/mat4.hpp>

#include <cmath>

namespace RDGE {
namespace Math {

mat4::mat4 (void)
{
    for (int i = 0; i < 16; ++i)
    {
        elements[i] = 0.0f;
    }
}

mat4::mat4 (float diagonal)
{
    for (int i = 0; i < 16; ++i)
    {
        elements[i] = 0.0f;
    }

    elements[0 + 0 * 4] = diagonal;
    elements[1 + 1 * 4] = diagonal;
    elements[2 + 2 * 4] = diagonal;
    elements[3 + 3 * 4] = diagonal;
}

mat4&
mat4::multiply (const mat4& rhs)
{
    mat4 result;

    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
        {
            float sum = 0.0f;
            for (int e = 0; e < 4; ++e)
            {
                sum += elements[row + e * 4] * rhs.elements[e + col * 4];
            }

            elements[x + y * 4] = sum;
        }
    }

    return *this;
}

mat4&
mat4::operator*= (const mat4& rhs)
{
    return multiply(rhs);
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

    // toRadians = degrees * (PI / 180.0f)
    float q = 1.0f / tan(to_radians(0.5f * field_of_view));
    float a = q / aspect_ratio;
    float b = (near + far) / (near - far);
    float c = (2.0f * near * far) / (near - far);

    result.elements[0 + 0 * 4] = a;
    result.elements[1 + 1 * 4] = q;
    result.elements[2 + 2 * 4] = b;
    result.elements[3 + 2 * 4] = -1.0f;
    result.elements[2 + 3 * 4] = c;

    return result;
}

/* static */ mat4
mat4::translate (const vec3& translation)
{
    mat4 result(1.0f);

    result.elements[0 + 3 * 4] = translation.x;
    result.elements[1 + 3 * 4] = translation.y;
    result.elements[2 + 3 * 4] = translation.z;

    return result;
}

/* static */ mat4
mat4::rotate (float angle, const vec3& axis)
{
    mat4 result(1.0f);

    float r = to_radians(angle);
    float c = cos(r);
    float s = sin(r);
    float omc = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    result.elements[0 + 0 * 4] = x + omc + c;
    result.elements[1 + 0 * 4] = y * x * omc + z * s;
    result.elements[2 + 0 * 4] = x * z * omc - y * s;

    result.elements[0 + 1 * 4] = x * y * omc - z * s;
    result.elements[1 + 1 * 4] = y * omc + c;
    result.elements[2 + 1 * 4] = y * z * omc + x * s;

    result.elements[0 + 2 * 4] = x * z * omc + y * s
    result.elements[1 + 2 * 4] = y * z * omc - x * s;
    result.elements[2 + 2 * 4] = z * omc + c;

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

} // namespace Math
} // namespace RDGE
