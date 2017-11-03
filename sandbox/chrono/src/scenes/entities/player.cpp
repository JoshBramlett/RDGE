#include "player.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

#include "../../asset_enums.hpp"
#include "../../globals.hpp"

#define CHRONO_ADD_SWORD 0

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

namespace {

// image pixels/meter
float base_asset_ppm = 16.f;

// sprite animations
CardinalDirectionArray<Animation> s_idle;
CardinalDirectionArray<Animation> s_walk;
CardinalDirectionArray<Animation> s_run;
CardinalDirectionArray<Animation> s_sheathe;
CardinalDirectionArray<Animation> s_fight;
CardinalDirectionArray<Animation> s_attack;

} // anonymous namespace

Player::Player (void)
{
    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_spritesheet_player);

    static bool once [[gnu::unused]] = [&sheet](void) {
        float scale = g_game.ppm / base_asset_ppm;
        s_idle[Direction::UP]    = sheet.GetAnimation(player_animation_idle_back, scale);
        s_idle[Direction::RIGHT] = sheet.GetAnimation(player_animation_idle_right, scale);
        s_idle[Direction::DOWN]  = sheet.GetAnimation(player_animation_idle_front, scale);
        s_idle[Direction::LEFT]  = sheet.GetAnimation(player_animation_idle_left, scale);

        s_walk[Direction::UP]    = sheet.GetAnimation(player_animation_walk_back, scale);
        s_walk[Direction::RIGHT] = sheet.GetAnimation(player_animation_walk_right, scale);
        s_walk[Direction::DOWN]  = sheet.GetAnimation(player_animation_walk_front, scale);
        s_walk[Direction::LEFT]  = sheet.GetAnimation(player_animation_walk_left, scale);

        s_run[Direction::UP]    = sheet.GetAnimation(player_animation_run_back, scale);
        s_run[Direction::RIGHT] = sheet.GetAnimation(player_animation_run_right, scale);
        s_run[Direction::DOWN]  = sheet.GetAnimation(player_animation_run_front, scale);
        s_run[Direction::LEFT]  = sheet.GetAnimation(player_animation_run_left, scale);

        s_sheathe[Direction::UP]    = sheet.GetAnimation(player_animation_sheathe_back, scale);
        s_sheathe[Direction::RIGHT] = sheet.GetAnimation(player_animation_sheathe_right, scale);
        s_sheathe[Direction::DOWN]  = sheet.GetAnimation(player_animation_sheathe_front, scale);
        s_sheathe[Direction::LEFT]  = sheet.GetAnimation(player_animation_sheathe_left, scale);

        s_fight[Direction::UP]    = sheet.GetAnimation(player_animation_fight_stance_back, scale);
        s_fight[Direction::RIGHT] = sheet.GetAnimation(player_animation_fight_stance_right, scale);
        s_fight[Direction::DOWN]  = sheet.GetAnimation(player_animation_fight_stance_front, scale);
        s_fight[Direction::LEFT]  = sheet.GetAnimation(player_animation_fight_stance_left, scale);

        s_attack[Direction::UP]    = sheet.GetAnimation(player_animation_attack_back, scale);
        s_attack[Direction::RIGHT] = sheet.GetAnimation(player_animation_attack_right, scale);
        s_attack[Direction::DOWN]  = sheet.GetAnimation(player_animation_attack_front, scale);
        s_attack[Direction::LEFT]  = sheet.GetAnimation(player_animation_attack_left, scale);
        return true;
    }();

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(), sheet.texture);
    this->facing = Direction::SOUTH;
    m_currentAnimation = &s_idle[this->facing];
}

void
Player::InitPhysics (CollisionGraph& graph, const math::vec2& pos)
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
        fprof.restitution = 0.8f;
        fprof.filter.category = chrono_collision_category_player_hitbox;
        fprof.filter.mask = chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_environment_static;

        polygon p(0.5f, 1.f);
        fprof.shape = &p;
        this->hitbox = body->CreateFixture(fprof);
    }

    {
        // directional sensors
        fixture_profile fprof;
        fprof.is_sensor = true;
        fprof.filter.category = chrono_collision_category_player_sensor_directional;
        fprof.filter.mask = chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_environment_triggers;

        circle c(math::vec2(-0.5f, 0.f), 1.f);
        fprof.shape = &c;
        this->dir_sensors[Direction::LEFT] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.5f, 0.f), 1.f);
        fprof.shape = &c;
        this->dir_sensors[Direction::RIGHT] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.f, 1.f), 0.5f);
        fprof.shape = &c;
        this->dir_sensors[Direction::UP] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.f, -1.f), 0.5f);
        fprof.shape = &c;
        this->dir_sensors[Direction::DOWN] = body->CreateFixture(fprof);
    }

#if (CHRONO_ADD_SWORD)
    // players sword
    bprof.prevent_rotation = false;
    bprof.position += { 0.f, 0.75f };
    sword = graph.CreateBody(bprof);

    polygon s(0.125f, 0.75f);
    fprof.shape = &s;
    fprof.density = 1.f;
    fprof.restitution = 0.8f;
    sword_hitbox = sword->CreateFixture(fprof);

    auto j = graph.CreateRevoluteJoint(body, sword, vec2(0.f, 0.f));
    j->SetMotorSpeed(3 * 3.14f);
    j->SetMaxMotorTorque(10000.f);
    j->EnableMotor();

    //j->SetLimits(-90.f, 90.f);
    //j->EnableLimits();
#endif
}

void
Player::OnEvent (const Event& event)
{
    m_handler.OnEvent(event);

    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating())
        {
            return; // ignore repeating events - nothing has changed
        }

        if (args.PhysicalKey() == ScanCode::J)
        {
            SET_FLAG(args.IsKeyPressed(), m_flags, RUN_BUTTON_PRESSED);
        }

        if (args.PhysicalKey() == ScanCode::K)
        {
            SET_FLAG(args.IsKeyPressed(), m_flags, ATTACK_BUTTON_PRESSED);
        }
    }
}

void
Player::OnUpdate (const delta_time& dt)
{
    uint32 ticks = dt.ticks;

    math::vec2 desired_velocity = m_direction;
    if (m_flags & ATTACKING)
    {
        desired_velocity *= 30.f;

        if (m_currentAnimation->IsFinished())
        {
            m_currentAnimation->Reset();
            m_flags &= ~ATTACKING;
        }
    }
    else
    {
        auto p = m_handler.Calculate();
        m_direction = p.first;
        this->facing = p.second;

        bool is_moving = !m_direction.is_zero();
        if (m_flags & ATTACK_BUTTON_PRESSED)
        {
            m_currentAnimation = &s_attack[this->facing];
            desired_velocity *= 30.f;

            m_flags |= ATTACKING;
        }
        else if (is_moving)
        {
            if (m_flags & RUN_BUTTON_PRESSED)
            {
                m_currentAnimation = &s_run[this->facing];
                desired_velocity *= 20.f;
            }
            else
            {
                m_currentAnimation = &s_walk[this->facing];
                desired_velocity *= 10.f;
            }
        }
        else
        {
            ticks = 0;
            m_currentAnimation = &s_idle[this->facing];
        }
    }

    math::vec2 delta = desired_velocity - body->linear.velocity;
    math::vec2 impulse = delta * body->linear.mass;

    if (ticks == 0)
    {
        // abrupt stop
        impulse *= 9.5f;
    }

    body->ApplyForce(impulse);
    //body->linear.velocity = desired_velocity;

    auto& frame = this->m_currentAnimation->GetFrame(ticks);
    math::vec2 pos((this->hitbox->GetWorldCenter() * g_game.ppm) - frame.origin);

    vops::SetPosition(this->sprite->vertices, pos, frame.size);
    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}

math::vec2
Player::GetWorldCenter (void) const noexcept
{
    return hitbox->GetWorldCenter();
}
