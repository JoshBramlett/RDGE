#include "duck.hpp"

#include "../test.hpp"
#include "player.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

Duck::Duck (TestScene* parent, const math::vec3& position)
    : m_parent(parent)
{
    SpriteSheet sheet("res/enemies.json", Window::Current().IsHighDPI());

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation("duck_back");
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation("duck_right");
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation("duck_front");
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation("duck_left");

    this->sprite = sheet.CreateSprite("duck_front_1", position);
    //this->sprite->debug_bounds.show = true;

    facing = Direction::SOUTH;
    current_animation = &cd_anim_walk[facing];
}

void
Duck::InitPhysics (CollisionGraph& graph, float inv_ratio)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    body = graph.CreateBody(bprof);

    polygon::PolygonData data;
    data[0] = { sprite->vertices[0].pos.x * inv_ratio, sprite->vertices[0].pos.y * inv_ratio };
    data[1] = { sprite->vertices[1].pos.x * inv_ratio, sprite->vertices[1].pos.y * inv_ratio };
    data[2] = { sprite->vertices[2].pos.x * inv_ratio, sprite->vertices[2].pos.y * inv_ratio };
    data[3] = { sprite->vertices[3].pos.x * inv_ratio, sprite->vertices[3].pos.y * inv_ratio };
    auto p = polygon(data, 4);

    fixture_profile fprof;
    fprof.shape = &p;
    fprof.density = 1.f;
    fprof.restitution = 0.1f;

    body->CreateFixture(fprof);
}

void
Duck::OnUpdate (const delta_time& dt)
{
    auto d = m_parent->player.GetWorldCenter() - body->GetWorldCenter();
    if (d.self_dot() > math::square(2.5f))
    {
        math::vec2 d_normal = d.normalize();
        math::vec2 desired_velocity = d_normal * 10.f;

        math::vec2 delta = desired_velocity - body->linear.velocity;
        math::vec2 impulse = delta * body->linear.mass;
        body->ApplyForce(impulse);
    }

    auto& frame = current_animation->GetFrame(dt.ticks);

    math::vec2 pos = body->GetWorldCenter() * 64.f;
    pos.x -= frame.size.w * 0.5f;
    pos.y -= frame.size.h * 0.5f;
    vops::SetPosition(sprite->vertices, pos);

    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}
