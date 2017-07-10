#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace rdge {

using namespace rdge::math;

OrthographicCamera::OrthographicCamera (void)
{
    auto vp = opengl::GetViewport();
    this->viewport_size = { vp[2], vp[3] };

    Update();
}

OrthographicCamera::OrthographicCamera (float width, float height)
    : viewport_size(width, height)
{
    Update();
}

void
OrthographicCamera::Update (void)
{
    this->projection = mat4::orthographic(this->zoom * -(this->viewport_size.w / 2.f),
                                          this->zoom * (this->viewport_size.w / 2.f),
                                          this->zoom * -(this->viewport_size.h / 2.f),
                                          this->zoom * (this->viewport_size.h / 2.f),
                                          this->near,
                                          this->far);
    this->view = mat4::look_at(this->position,                   // camera eye
                               this->position + this->direction, // what to look at
                               this->up);                        // world "up"
    this->combined = this->projection * this->view;
}

void
OrthographicCamera::Translate (const vec2& displacement)
{
    this->position.x += displacement.x;
    this->position.y += displacement.y;
}

void
OrthographicCamera::SetPosition (const vec2& position)
{
    this->position.x = position.x;
    this->position.y = position.y;
}

void
OrthographicCamera::Rotate (float angle)
{
    auto rotation = mat4::rotation(angle, this->direction);
    this->direction.transform(rotation);
    this->up.transform(rotation);
}

} // namespace rdge
