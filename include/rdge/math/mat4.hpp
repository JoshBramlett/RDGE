//! \headerfile <rdge/math/mat4.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>

#include <ostream>
#include <sstream>
#include <iomanip>

/* TODO
 *
 * A lot of this I don't understand, so I'll have to go back later to add some
 * color to what these methods do.  Also, there's a bit of difference between
 * the tutorial and the current version of the file, which I'll need to consider
 * implementing.
 *
 * Also, I need to finish documentation.
 */

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

//! \struct mat4
//! \brief Represents a 4x4 matrix, ordered by column major
struct mat4
{
    union
    {
        //! \var elements Data container array
        float elements[16];
        //! \var column Data container by column
        vec4 columns[4];
    };

    //! \brief mat4 ctor
    //! \details Initializes all elements to zero
    mat4 (void);

    //! \brief mat4 ctor
    //! \details Initializes all elements to zero, and generates an indentity
    //!          matrix from the provided diagonal
    //! \param [in] diagonal Value to set in the identity matrix
    explicit mat4 (float diagonal);

    //! \brief mat4 dtor
    ~mat4 (void) { }

    //! \brief mat4 Copy ctor
    //! \details Default-copyable
    mat4 (const mat4&) noexcept = default;

    //! \brief mat4 Move ctor
    //! \details Default-movable
    mat4 (mat4&&) noexcept = default;

    //! \brief mat4 Copy Assignment Operator
    //! \details Default-copyable
    mat4& operator= (const mat4&) noexcept = default;

    //! \brief mat4 Move Assignment Operator
    //! \details Default-movable
    mat4& operator= (mat4&&) noexcept = default;

    //! \brief Multiplication assignment operator
    //! \details Multiplies by another mat4 matrix
    mat4& operator*= (const mat4& rhs);

    //! \brief Multiply object by another mat4 matrix
    //! \param [in] other Matrix to multiply
    //! \returns Reference to current object
    mat4& multiply (const mat4& rhs);

    //! \brief Create an identity matrix
    //! \details Initializes diagonal to 1.0f
    //! \returns New identity matrix
    static mat4 identity (void);

    //! \brief Create an orthographic matrix
    //! \details TODO Describe
    //! \param [in] left Left clipping plane
    //! \param [in] right Right clipping plane
    //! \param [in] bottom Bottom clipping plane
    //! \param [in] top Top clipping plane
    //! \param [in] near Near clipping plane
    //! \param [in] far Far clipping plane
    //! \returns New orthographic matrix
    static mat4 orthographic (
                              float left,
                              float right,
                              float bottom,
                              float top,
                              float near,
                              float far
                             );

    //! \brief Create a perspective matrix
    //! \details Transformation between the camaera space and the normal device
    //!          coordinates.  The camera view frustrum is converted to a cube,
    //!          and everything within is transformed accordingly.  All vertices
    //!          are shifted towards the "eye" vertex.
    //! \param [in] fov Field of view, in degrees (the amount of "zoom")
    //! \param [in] aspect_ratio Aspect ratio of the drawing screen
    //! \param [in] near Near clipping plane
    //! \param [in] far Far clipping plane
    //! \returns New perspective matrix
    static mat4 perspective (
                             float fov,
                             float aspect_ratio,
                             float near,
                             float far
                            );

    //! \brief Create a translation matrix
    //! \details Geometric transformation that moves every vertex in a model by
    //!          the same amount in the same direction.
    //! \param [in] translation Three dimensional vector
    //! \returns New translation matrix
    //! \see https://en.wikipedia.org/wiki/Translation_(geometry)
    static mat4 translation (const vec3& translation);

    static mat4 rotate (float angle, const vec3& axis);

    static mat4 scale (const vec3& scale);
};

//! \brief mat4 multiplication operator
//! \param [in] lhs First mat4
//! \param [in] rhs Second mat4
//! \returns Multiplied mat4 result
inline mat4 operator* (const mat4& lhs, const mat4& rhs)
{
    mat4 result;
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            int idx = x + y * 4;
            for (int e = 0; e < 4; ++e)
            {
                result.elements[idx] += lhs.elements[x + e * 4] * rhs.elements[e + y * 4];
            }
        }
    }

    return result;
}

//! \brief mat4-vec4 multiplication operator
//! \param [in] lhs mat4 to multiply
//! \param [in] rhs vec4 to multiply
//! \returns Multiplied vec4 result
inline vec4 operator* (const mat4& lhs, const vec4& rhs)
{
    auto col = lhs.columns;
    return vec4
    {
        col[0].x * rhs.x + col[1].x * rhs.y + col[2].x * rhs.z + col[3].x * rhs.w,
        col[0].y * rhs.x + col[1].y * rhs.y + col[2].y * rhs.z + col[3].y * rhs.w,
        col[0].z * rhs.x + col[1].z * rhs.y + col[2].z * rhs.z + col[3].z * rhs.w,
        col[0].w * rhs.x + col[1].w * rhs.y + col[2].w * rhs.z + col[3].w * rhs.w
    };
}

//! \brief mat4-vec3 multiplication operator
//! \param [in] lhs mat4 to multiply
//! \param [in] rhs vec3 to multiply
//! \returns Multiplied vec3 result
inline vec3 operator* (const mat4& lhs, const vec3& rhs)
{
    auto col = lhs.columns;
    return vec3
    {
        col[0].x * rhs.x + col[1].x * rhs.y + col[2].x * rhs.z + col[3].x,
        col[0].y * rhs.x + col[1].y * rhs.y + col[2].y * rhs.z + col[3].y,
        col[0].z * rhs.x + col[1].z * rhs.y + col[2].z * rhs.z + col[3].z
    };
}

//! \brief mat4 stream output operator
//! \param [in] os Output stream
//! \param [in] matrix mat4 to write to the stream
//! \returns Output stream
inline std::ostream& operator<< (std::ostream& os, const mat4& matrix)
{
    std::stringstream ss;
    ss << "mat4: ["
       << std::fixed << std::setprecision(5);

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
