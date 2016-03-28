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
    //! \var VECTOR_COUNT Number of vectors (rows or columns)
    static constexpr RDGE::UInt8 VECTOR_COUNT = 4;
    //! \var ELEMENT_COUNT Total number of elements
    static constexpr RDGE::UInt8 ELEMENT_COUNT = 16;

    union
    {
        //! \var elements Data container array
        float elements[ELEMENT_COUNT];
        vec4 columns[VECTOR_COUNT];
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
    mat4& operator=(const mat4&) noexcept = default;

    //! \brief mat4 Move Assignment Operator
    //! \details Default-movable
    mat4& operator=(mat4&&) noexcept = default;

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
    //! \param [in] left Left bound
    //! \param [in] right Right bound
    //! \param [in] bottom Bottom bound
    //! \param [in] top Top bound
    //! \param [in] near Near bound
    //! \param [in] far Far bound
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
    //! \details TODO Describe
    //! \param [in] field_of_view TODO Describe
    //! \param [in] aspect_ratio Aspect ratio of the drawing screen
    //! \param [in] near Near bound
    //! \param [in] far Far bound
    //! \returns New perspective matrix
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
