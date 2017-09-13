//! \headerfile <rdge/graphics/orthographic_camera.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class OrthographicCamera
//! \brief Camera with an orthographic projection
//! \details The camera region is represented with point [0,0] in the center of
//!          the screen, and the edges being half the viewport size in both negative
//!          (left and bottom) and positive (right and top) directions.
class OrthographicCamera
{
public:
    static constexpr float DEFAULT_ZOOM = 1.f;

    //! \brief OrthographicCamera default ctor
    //! \details Creates an orthographic camera using the width and height values
    //!          queried from the OpenGL viewport.
    OrthographicCamera (void);

    //! \brief OrthographicCamera default ctor
    //! \details Creates an orthographic camera with custom width and height values.
    //! \param [in] width Viewport width
    //! \param [in] height Viewport height
    explicit OrthographicCamera (float width, float height);

    //! \brief Build matrices from the cached data
    //! \details Rebuilds the view and projection matrices, and multiplies them to
    //!          create the combined matrix.
    void Update (void);

    //! \brief Translate the position of the camera
    //! \param [in] displacement Position offset vector
    //! \note Update will need to be called afterwards to apply the translation.
    void Translate (const math::vec2& displacement);

    //! \brief Set the position of the camera
    //! \param [in] position Position
    //! \note Update will need to be called afterwards to apply the translation.
    void SetPosition (const math::vec2& position);

    //! \brief Rotate the camera by the provided angle
    //! \details Rotation is performed on the z-axis.
    //! \param [in] angle Angle (in degrees) to rotate
    //! \note Update will need to be called afterwards to apply the rotation.
    void Rotate (float angle);

public:
    math::vec3 position  = math::vec3::ZERO; //!< Camera position
    math::vec3 direction = -math::vec3::Z;   //!< Unit vector camera direction
    math::vec3 up        = math::vec3::Y;    //!< Unit vector world "up"

    math::mat4 projection;       //!< Projection matrix
    math::mat4 view;             //!< View matrix
    math::mat4 combined;         //!< Combined projection/view matrix (in that order)
    math::mat4 inverse_combined; //!< Inverse of the combined matrix

    float near = 0.f;          //!< Near clipping plane
    float far  = 3.f;          //!< Far clipping plane
    float zoom = DEFAULT_ZOOM; //!< Projection zoom level

    math::vec2 viewport_size; //!< Cached viewport size
};

} // namespace rdge
