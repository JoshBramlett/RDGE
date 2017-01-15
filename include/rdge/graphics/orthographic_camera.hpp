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
//! \details
class OrthographicCamera
{
public:
    OrthographicCamera (void);

    explicit OrthographicCamera (float width, float height);

    void Update (void);

    void Translate (const math::vec2& displacement);

    void Rotate (float angle);

public:
    math::vec3 position  = math::vec3::ZERO; //!< Camera position
    math::vec3 direction = -math::vec3::Z;   //!< Unit vector camera direction
    math::vec3 up        = math::vec3::Y;    //!< Unit vector world "up"

    math::mat4 projection;       //!< Projection matrix
    math::mat4 view;             //!< View matrix
    math::mat4 combined;         //!< Combined view/projection matrix
    math::mat4 inverse_combined; //!< Inverse of the combined matrix

    float near = 0.f;   //!< Near clipping plane
    float far  = 100.f; //!< Far clipping plane
    float zoom = 1.f;   //!< Projection zoom level

    math::vec2 viewport_size; //!< Cached viewport size
};

} // namespace rdge
