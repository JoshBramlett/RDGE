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

// image pixels/meter
float base_asset_ppm = 16.f;

// sprite animations
CardinalDirectionArray<Animation> s_walk;

} // anonymous namespace

Duck::Duck (TestScene* parent)
    : m_parent(parent)
{
    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_spritesheet_enemies);

    static bool once [[gnu::unused]] = [&sheet](void) {
        float scale = g_game.ppm / base_asset_ppm;
        s_walk[Direction::UP]    = sheet.GetAnimation(enemies_animation_duck_back, scale);
        s_walk[Direction::RIGHT] = sheet.GetAnimation(enemies_animation_duck_right, scale);
        s_walk[Direction::DOWN]  = sheet.GetAnimation(enemies_animation_duck_front, scale);
        s_walk[Direction::LEFT]  = sheet.GetAnimation(enemies_animation_duck_left, scale);
        return true;
    }();

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(), sheet.texture);
    this->facing = Direction::SOUTH;
    m_currentAnimation = &s_walk[this->facing];
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
    bprof.linear_damping = 0.5f;
    this->body = graph.CreateBody(bprof);

    {
        // hitbox
        fixture_profile fprof;
        fprof.density = 1.f;
        fprof.restitution = 0.9f;
        fprof.filter.category = chrono_collision_category_enemy_hitbox;
        fprof.filter.mask = chrono_collision_category_player_hitbox |
                            chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_environment_static;

        auto c = circle(0.5f);
        fprof.shape = &c;
        this->hitbox = body->CreateFixture(fprof);
    }
}

void
Duck::OnUpdate (const delta_time& dt)
{
    auto d = m_parent->player.GetWorldCenter() - body->GetWorldCenter();
    m_currentAnimation = &s_walk[GetDirection(d)];

    if (d.self_dot() > math::square(2.5f))
    {
        math::vec2 d_normal = d.normalize();
        math::vec2 desired_velocity = d_normal * 10.f;

        math::vec2 delta = desired_velocity - body->linear.velocity;
        math::vec2 impulse = delta * body->linear.mass;
        body->ApplyForce(impulse);
    }

    auto& frame = m_currentAnimation->GetFrame(dt.ticks);
    math::vec2 pos((this->body->GetWorldCenter() * g_game.ppm) - frame.origin);

    vops::SetPosition(this->sprite->vertices, pos, frame.size);
    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}

math::vec2
Duck::GetWorldCenter (void) const noexcept
{
    return hitbox->GetWorldCenter();
}
