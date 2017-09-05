//! \headerfile <rdge/math/mat4.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>

/* TODO
 *
 * A lot of this I don't understand, so I'll have to go back later to add some
 * color to what these methods do.  Also, there's a bit of difference between
 * the tutorial and the current version of the file, which I'll need to consider
 * implementing.
 *
 * Also, I need to finish documentation.
 */

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct mat4
//! \brief Represents a 4x4 matrix, ordered by column major
struct mat4
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    union
    {
        float elements[16]; //!< Container by element
        vec4 columns[4];    //!< Container by column
    };

#pragma GCC diagnostic pop

    //! \brief mat4 default ctor
    //! \details Zero initialization
    mat4 (void);

    //! \brief mat4 subscript operator
    //! \param [in] index Index of column
    //! \returns Reference to vec4 column
    vec4& operator[] (uint8 index) noexcept;

    //! \brief mat4 subscript operator
    //! \param [in] index Index of column
    //! \returns Const reference to vec4 column
    const vec4& operator[] (uint8 index) const noexcept;

    //! \brief mat4 memberwise multiplication
    //! \param [in] rhs mat4 to multiply
    //! \returns Reference to self
    mat4& operator*= (const mat4& rhs);

    //! \brief Create an inverse of the matrix
    //! \returns mat4 containing the inverse of the parent
    //! \see http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
    mat4 inverse (void);

    //! \brief Create an identity matrix
    //! \details Initializes diagonal to 1.0f
    //! \returns Identity matrix
    static mat4 identity (void);

    //! \brief Create an orthographic matrix
    //! \details Orthographic projection involves throwing away the coordinate
    //!          which is perpendicular to the surface.
    //! \param [in] left Left clipping plane
    //! \param [in] right Right clipping plane
    //! \param [in] bottom Bottom clipping plane
    //! \param [in] top Top clipping plane
    //! \param [in] near Near clipping plane
    //! \param [in] far Far clipping plane
    //! \returns Orthographic matrix
    static mat4 orthographic (float left,
                              float right,
                              float bottom,
                              float top,
                              float near,
                              float far);

    //! \brief Create a perspective matrix
    //! \details Transformation between the camaera space and the normal device
    //!          coordinates.  The camera view frustrum is converted to a cube,
    //!          and everything within is transformed accordingly.  All vertices
    //!          are shifted towards the "eye" vertex.
    //! \param [in] fov Field of view, in degrees (the amount of "zoom")
    //! \param [in] aspect_ratio Aspect ratio of the drawing screen
    //! \param [in] near Near clipping plane
    //! \param [in] far Far clipping plane
    //! \returns Perspective matrix
    static mat4 perspective (float fov, float aspect_ratio, float near, float far);

    //! \brief Create a translation matrix
    //! \details Geometric transformation that moves every vertex in a model by
    //!          the same amount in the same direction.
    //! \param [in] translation Vector of x, y, and z values to translate
    //! \returns Translation matrix
    //! \see https://en.wikipedia.org/wiki/Translation_(geometry)
    static mat4 translation (const vec3& translation);

    //! \brief Create a rotation matrix
    //! \details The rotation matrix is generated from the axis & angle representation.
    //! \param [in] angle Magnitude of the rotation
    //! \param [in] axis Unit vector indicating the direction of an axis of rotation
    //! \returns Rotation matrix
    //! \see https://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
    static mat4 rotation (float angle, const vec3& axis);

    //! \brief Create a scale matrix
    //! \details Geometric transformation that multiplies every vertex by the x, y,
    //!          and z values provided.
    //! \param [in] scale Vector of x, y, and z values to scale
    //! \returns Scale matrix
    static mat4 scale (const vec3& scale);

    //! \brief Create a LookAt view matrix
    //! \details View matrix which emulates the behavior of the gluLookAt function,
    //!          which rotates the world around the camera.  After the matrix is
    //!          applied the camera will be set to [0,0] and will point at the
    //!          provided vertex.
    //! \param [in] eye Position of the camera
    //! \param [in] center Position camera will "look at"
    //! \param [in] up Upwards direction of the world coordinates
    //! \returns View matrix
    static mat4 look_at (const vec3& eye, const vec3& center, const vec3& up);
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
inline vec4 operator* (const mat4& mat, const vec4& vec)
{
    return vec4
    {
        (mat[0].x * vec.x) + (mat[1].x * vec.y) + (mat[2].x * vec.z) + (mat[3].x * vec.w),
        (mat[0].y * vec.x) + (mat[1].y * vec.y) + (mat[2].y * vec.z) + (mat[3].y * vec.w),
        (mat[0].z * vec.x) + (mat[1].z * vec.y) + (mat[2].z * vec.z) + (mat[3].z * vec.w),
        (mat[0].w * vec.x) + (mat[1].w * vec.y) + (mat[2].w * vec.z) + (mat[3].w * vec.w)
    };
}

//! \brief mat4 stream output operator
//! \param [in] os Output stream
//! \param [in] matrix mat4 to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const mat4& matrix);

} // namespace math
} // namespace rdge
