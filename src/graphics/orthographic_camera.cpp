#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/util/logger.hpp>

namespace rdge {

using namespace rdge::math;

OrthographicCamera::OrthographicCamera (void)
{
    auto vp = opengl::GetViewport();
    this->viewport_size = { vp[2], vp[3] };
    DLOG() << "Camera default viewport: " << viewport_size;

    Update();
}

OrthographicCamera::OrthographicCamera (float width, float height)
    : viewport_size(width, height)
{
    DLOG() << "Camera custom viewport: " << viewport_size;

    Update();
}

void
OrthographicCamera::Update (void)
{
    // half extents around origin
    float he_x = zoom * (viewport_size.w * 0.5f);
    float he_y = zoom * (viewport_size.h * 0.5f);

    this->projection = mat4::orthographic(-he_x, he_x, -he_y, he_y, near, far);
    this->view = mat4::look_at(position,             // camera eye
                               position + direction, // what to look at
                               up);                  // world "up"
    this->combined = projection * view;

    // accomodate rotations in bounds
    float adj_x = he_x * math::abs(up.y) + he_y * math::abs(up.x);
    float adj_y = he_y * math::abs(up.y) + he_x * math::abs(up.x);

    math::vec2 lo(position.x - adj_x, position.y - adj_y);
    math::vec2 hi(position.x + adj_x, position.y + adj_y);
    this->bounds = physics::aabb(lo, hi);
}

void
OrthographicCamera::Translate (const vec2& displacement)
{
    this->position.x += displacement.x;
    this->position.y += displacement.y;
}

void
OrthographicCamera::SetPosition (const vec2& pos)
{
    this->position.x = pos.x;
    this->position.y = pos.y;
}

void
OrthographicCamera::Rotate (float angle)
{
    auto rotation = mat4::rotation(angle, direction);
    this->direction.transform(rotation);
    this->up.transform(rotation);
}

} // namespace rdge
