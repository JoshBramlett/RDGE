//! \headerfile <rdge/graphics/orthographic_camera.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/09/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class OrthographicCamera
//! \details
class OrthographicCamera
{
public:
    OrthographicCamera (void) = default;

    explicit OrthographicCamera (float width, float height)
        : viewport_width(width)
        , viewport_height(height)
    {

    }

    void Update (void)
    {
        this->projection = math::math4::orthographic(zoom * (-viewport_width / 2),
                                                     zoom * (viewport_width / 2),
                                                     zoom * (-viewport_height / 2),
                                                     zoom * (viewport_height / 2),
                                                     near,
                                                     far);
    }

public:
    math::vec3 position  = { 0.f };
    math::vec3 direction = { 0.f, 0.f, -1.f };
    math::vec3 up        = { 0.f, 1.f, 0.f };

    math::mat4 projection;
    math::mat4 view;
    math::mat4 combined;
    math::mat4 inverse_combined;

    float near = 0.f;
    float far  = 100.f;
    float zoom = 1.f; // <-- only in libgdx orthographic camera

    float viewport_width  = 0.f;
    float viewport_height = 0.f;
};

} // namespace rdge
