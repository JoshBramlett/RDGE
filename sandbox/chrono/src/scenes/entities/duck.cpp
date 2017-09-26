#include "duck.hpp"

#include "../test.hpp"
#include "player.hpp"
#include "../../chrono.hpp"

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
    PackFile pack("res/chrono.data");
    auto sheet = pack.GetSpriteSheet(chrono_asset_spritesheet_enemies);

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation(enemies_animation_duck_back);
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation(enemies_animation_duck_right);
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation(enemies_animation_duck_front);
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation(enemies_animation_duck_left);

    this->sprite = std::make_shared<Sprite>(position, vec2(64.f, 64.f), sheet.texture);

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

#if 0
    polygon::PolygonData data;
    data[0] = { sprite->vertices[0].pos.x * inv_ratio, sprite->vertices[0].pos.y * inv_ratio };
    data[1] = { sprite->vertices[1].pos.x * inv_ratio, sprite->vertices[1].pos.y * inv_ratio };
    data[2] = { sprite->vertices[2].pos.x * inv_ratio, sprite->vertices[2].pos.y * inv_ratio };
    data[3] = { sprite->vertices[3].pos.x * inv_ratio, sprite->vertices[3].pos.y * inv_ratio };
    auto p = polygon(data, 4);
#else
    const auto pos = vops::GetPosition(sprite->vertices) * inv_ratio;
    const auto size = vops::GetSize(sprite->vertices) * inv_ratio;

    auto p = circle({pos.x, pos.y}, size.w * 0.5f);
#endif

    fixture_profile fprof;
    fprof.shape = &p;
    fprof.density = 1.f;
    fprof.restitution = 0.9f;

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
