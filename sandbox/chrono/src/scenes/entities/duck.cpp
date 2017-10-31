#include "duck.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

#include "player.hpp"
#include "../test.hpp"
#include "../../asset_enums.hpp"
#include "../../globals.hpp"

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

namespace {

    float base_asset_ppm = 16.f;

} // anonymous namespace

Duck::Duck (TestScene* parent)
    : m_parent(parent)
{
    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_spritesheet_enemies);
    float scale = g_game.ppm / base_asset_ppm;

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation(enemies_animation_duck_back, scale);
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation(enemies_animation_duck_right, scale);
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation(enemies_animation_duck_front, scale);
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation(enemies_animation_duck_left, scale);

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(), sheet.texture);

    facing = Direction::SOUTH;
    current_animation = &cd_anim_walk[facing];
}

void
Duck::InitPhysics (CollisionGraph& graph, const math::vec2& pos)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.position = pos;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    body = graph.CreateBody(bprof);

    // TODO update spawn point
    auto p = circle(0.5f);

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
    current_animation = &cd_anim_walk[GetDirection(d)];

    if (d.self_dot() > math::square(2.5f))
    {
        math::vec2 d_normal = d.normalize();
        math::vec2 desired_velocity = d_normal * 10.f;

        math::vec2 delta = desired_velocity - body->linear.velocity;
        math::vec2 impulse = delta * body->linear.mass;
        body->ApplyForce(impulse);
    }

    auto& frame = current_animation->GetFrame(dt.ticks);

    math::vec2 pos((this->body->GetWorldCenter() * g_game.ppm) - frame.origin);
    vops::SetPosition(this->sprite->vertices, pos, frame.size);
    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}
