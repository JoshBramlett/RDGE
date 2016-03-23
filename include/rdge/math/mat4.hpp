//! \headerfile <rdge/math/mat4.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec3.hpp>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

// column major
struct mat4
{
    float elements[16]; // 4 * 4

    mat4 (void);

    explicit mat4 (float diagonal);

    mat4& multiply (const mat4& rhs);

    // TODO: Do operator*
    mat4& operator*= (const mat4& rhs);


    static mat4 identity (void);

    static mat4 orthographic (
                              float left,
                              float right,
                              float bottom,
                              float top,
                              float near,
                              float far
                             );

    static mat4 perspective (
                             float field_of_view,
                             float aspect_ratio,
                             float near,
                             float far
                            );

    static mat4 translate (const vec3& translation);

    static mat4 rotate (float angle, const vec3& axis);

    static mat4 scale (const vec3& scale);
};


} // namespace Math
} // namespace RDGE
