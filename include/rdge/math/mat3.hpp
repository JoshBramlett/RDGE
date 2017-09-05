//! \headerfile <rdge/math/mat3.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/31/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct mat3
//! \brief Represents a 3x3 matrix, ordered by column major
struct mat3
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    union
    {
        float elements[9]; //!< Container by element
        vec3 columns[3];   //!< Container by column
    };

#pragma GCC diagnostic pop

    //! \brief mat3 default ctor
    //! \details Zero initialization
    mat3 (void);

    //! \brief mat3 subscript operator
    //! \param [in] index Index of column
    //! \returns Reference to vec3 column
    vec3& operator[] (uint8 index) noexcept;

    //! \brief mat3 subscript operator
    //! \param [in] index Index of column
    //! \returns Const reference to vec3 column
    const vec3& operator[] (uint8 index) const noexcept;

    //! \brief Determinant of the matrix
    //! \details Determines if a linear equation solution exists.
    //! \returns The determinant, or zero if unsolvable
    float determinant (void) const noexcept;

    //! \brief Solve the system of linear equations Ax = b
    //! \details Inverse of this matrix (A) is multiplied on both sides, solving
    //!          and returning x.  (i.e. x = (A^-1)b)
    //! \param [in] b The b vector
    //! \returns The x vector
    vec3 solve (const vec3& b) const noexcept;

    //! \brief Solve the system of linear equations Ax = b for the mat2
    //! \details Inverse of this matrix (A) is multiplied on both sides, solving
    //!          and returning x.  (i.e. x = (A^-1)b)
    //! \param [in] b The b vector
    //! \returns The x vector
    vec2 solve (const vec2& b) const noexcept;

    //! \brief Create an identity matrix
    //! \details Initializes diagonal to 1.0f
    //! \returns Identity matrix
    static mat3 identity (void);
};

//! \brief mat3-vec3 multiplication operator
//! \param [in] lhs mat3 to multiply
//! \param [in] rhs vec3 to multiply
//! \returns Multiplied vec3 result
inline vec3 operator* (const mat3& mat, const vec3& vec)
{
    return vec3
    {
        (mat[0].x * vec.x) + (mat[1].x * vec.y) + (mat[2].x * vec.z),
        (mat[0].y * vec.x) + (mat[1].y * vec.y) + (mat[2].y * vec.z),
        (mat[0].z * vec.x) + (mat[1].z * vec.y) + (mat[2].z * vec.z)
    };
}

//! \brief mat3 stream output operator
std::ostream& operator<< (std::ostream& os, const mat3& matrix);

} // namespace math
} // namespace rdge
