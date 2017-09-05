//! \headerfile <rdge/math/mat2.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/31/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \struct mat2
//! \brief Represents a 2x2 matrix, ordered by column major
struct mat2
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    union
    {
        float elements[4]; //!< Container by element
        vec2 columns[2];   //!< Container by column
    };

#pragma GCC diagnostic pop

    //! \brief mat2 default ctor
    //! \details Zero initialization
    mat2 (void);

    //! \brief mat2 subscript operator
    //! \param [in] index Index of column
    //! \returns Reference to vec2 column
    vec2& operator[] (uint8 index) noexcept;

    //! \brief mat2 subscript operator
    //! \param [in] index Index of column
    //! \returns Const reference to vec2 column
    const vec2& operator[] (uint8 index) const noexcept;

    //! \brief Determinant of the matrix
    //! \details Determines if a linear equation solution exists.
    //! \returns The determinant, or zero if unsolvable
    float determinant (void) const noexcept;

    //! \brief Solve the system of linear equations Ax = b for the mat2
    //! \details Inverse of this matrix (A) is multiplied on both sides, solving
    //!          and returning x.  (i.e. x = (A^-1)b)
    //! \param [in] b The b vector
    //! \returns The x vector
    vec2 solve (const vec2& b) const noexcept;

    //! \brief Create an identity matrix
    //! \details Initializes diagonal to 1.0f
    //! \returns Identity matrix
    static mat2 identity (void);
};

//! \brief mat2-vec2 multiplication operator
//! \param [in] lhs mat2 to multiply
//! \param [in] rhs vec2 to multiply
//! \returns Multiplied vec2 result
inline vec2 operator* (const mat2& mat, const vec2& vec)
{
    return vec2
    {
        (mat[0].x * vec.x) + (mat[1].x * vec.y),
        (mat[0].y * vec.x) + (mat[1].y * vec.y)
    };
}

//! \brief mat2 stream output operator
std::ostream& operator<< (std::ostream& os, const mat2& matrix);

} // namespace math
} // namespace rdge
