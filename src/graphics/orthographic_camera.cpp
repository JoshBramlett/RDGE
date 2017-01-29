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
    this->projection = mat4::orthographic(zoom * -(viewport_size.w / 2.f),
                                          zoom * (viewport_size.w / 2.f),
                                          zoom * -(viewport_size.h / 2.f),
                                          zoom * (viewport_size.h / 2.f),
                                          near,
                                          far);
    this->view = mat4::look_at(position, (position + direction), up);
    this->combined = this->projection * this->view;
}

void
OrthographicCamera::Translate (const vec2& displacement)
{
    this->position += vec3(displacement, 0.f);
}

void
OrthographicCamera::Rotate (float angle)
{
    auto rotation = mat4::rotation(angle, this->direction);
    this->direction.affine_transform(rotation);
    this->up.affine_transform(rotation);
}

} // namespace rdge
