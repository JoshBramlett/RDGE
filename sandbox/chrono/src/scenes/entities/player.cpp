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

Player::Player (void)
{
    PackFile pack("res/chrono.data");
    auto sheet = pack.GetSpriteSheet(chrono_asset_spritesheet_player);

    //////////////////
    // idle animation
    //////////////////
    cd_anim_blink[Direction::UP]    = sheet.GetAnimation(player_animation_idle_back);
    cd_anim_blink[Direction::RIGHT] = sheet.GetAnimation(player_animation_idle_right);
    cd_anim_blink[Direction::DOWN]  = sheet.GetAnimation(player_animation_idle_front);
    cd_anim_blink[Direction::LEFT]  = sheet.GetAnimation(player_animation_idle_left);

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation(player_animation_walk_back);
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation(player_animation_walk_right);
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation(player_animation_walk_front);
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation(player_animation_walk_left);

    //////////////////
    // running animation
    //////////////////
    cd_anim_run[Direction::UP]    = sheet.GetAnimation(player_animation_run_back);
    cd_anim_run[Direction::RIGHT] = sheet.GetAnimation(player_animation_run_right);
    cd_anim_run[Direction::DOWN]  = sheet.GetAnimation(player_animation_run_front);
    cd_anim_run[Direction::LEFT]  = sheet.GetAnimation(player_animation_run_left);

    //////////////////
    // sheathe animation
    //////////////////
    cd_anim_sheathe[Direction::UP]    = sheet.GetAnimation(player_animation_sheathe_back);
    cd_anim_sheathe[Direction::RIGHT] = sheet.GetAnimation(player_animation_sheathe_right);
    cd_anim_sheathe[Direction::DOWN]  = sheet.GetAnimation(player_animation_sheathe_front);
    cd_anim_sheathe[Direction::LEFT]  = sheet.GetAnimation(player_animation_sheathe_left);

    //////////////////
    // fight stance animation
    //////////////////
    cd_anim_fight[Direction::UP]    = sheet.GetAnimation(player_animation_fight_stance_back);
    cd_anim_fight[Direction::RIGHT] = sheet.GetAnimation(player_animation_fight_stance_right);
    cd_anim_fight[Direction::DOWN]  = sheet.GetAnimation(player_animation_fight_stance_front);
    cd_anim_fight[Direction::LEFT]  = sheet.GetAnimation(player_animation_fight_stance_left);

    //////////////////
    // attack animation
    //////////////////
    cd_anim_attack[Direction::UP]    = sheet.GetAnimation(player_animation_attack_back);
    cd_anim_attack[Direction::RIGHT] = sheet.GetAnimation(player_animation_attack_right);
    cd_anim_attack[Direction::DOWN]  = sheet.GetAnimation(player_animation_attack_front);
    cd_anim_attack[Direction::LEFT]  = sheet.GetAnimation(player_animation_attack_left);

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(64.f, 128.f), sheet.texture);

    m_facing = Direction::SOUTH;
    current_animation = &cd_anim_blink[m_facing];
}

void
Player::InitPhysics (CollisionGraph& graph, float inv_ratio)
{
    inv_ratio *= 0.5f;

    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    bprof.linear_damping = 0.5f;
    body = graph.CreateBody(bprof);

    // players body
    polygon p(0.5f, 1.f);
    fixture_profile fprof;
    fprof.shape = &p;
    fprof.density = 1.f;
    fprof.restitution = 0.8f;
    hitbox = body->CreateFixture(fprof);

    bprof.prevent_rotation = false;
    bprof.position = { 0.f, 0.f };
    sword = graph.CreateBody(bprof);

    // players sword
    //vertices[0]=[-0.125, -2] normals[0]=[0, -1]
    //vertices[1]=[0.125, -2] normals[1]=[1, 0]
    //vertices[2]=[0.125, 2] normals[2]=[0, 1]
    //vertices[3]=[-0.125, 2] normals[3]=[-1, 0]

    polygon s(0.125f, 2.f);
    fprof.shape = &s;
    fprof.density = 1.f;
    fprof.restitution = 0.8f;
    sword_hitbox = sword->CreateFixture(fprof);

    auto j = graph.CreateRevoluteJoint(body, sword, vec2(0.f, 0.f));
    j->SetMotorSpeed(1 * 3.14f);
    j->SetMaxMotorTorque(10000.f);
    j->EnableMotor();

    //j->SetLimits(-90.f, 90.f);
    //j->EnableLimits();
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
            if (args.IsKeyPressed())
            {
                m_flags |= RUN_BUTTON_PRESSED;
            }
            else
            {
                m_flags &= ~RUN_BUTTON_PRESSED;
            }
        }

        if (args.PhysicalKey() == ScanCode::K)
        {
            if (args.IsKeyPressed())
            {
                m_flags |= ATTACK_BUTTON_PRESSED;
            }
            else
            {
                m_flags &= ~ATTACK_BUTTON_PRESSED;
            }
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

        if (current_animation->IsFinished())
        {
            current_animation->Reset();
            m_flags &= ~ATTACKING;
        }
    }
    else
    {
        auto p = m_handler.Calculate();
        m_direction = p.first;
        m_facing = p.second;

        bool is_moving = !m_direction.is_zero();
        if (m_flags & ATTACK_BUTTON_PRESSED)
        {
            current_animation = &cd_anim_attack[m_facing];
            desired_velocity *= 30.f;

            m_flags |= ATTACKING;
        }
        else if (is_moving)
        {
            if (m_flags & RUN_BUTTON_PRESSED)
            {
                current_animation = &cd_anim_run[m_facing];
                desired_velocity *= 20.f;
            }
            else
            {
                current_animation = &cd_anim_walk[m_facing];
                desired_velocity *= 10.f;
            }
        }
        else
        {
            ticks = 0;
            current_animation = &cd_anim_blink[m_facing];
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

    auto& frame = current_animation->GetFrame(ticks);

    math::vec2 pos = hitbox->GetWorldCenter() * 64.f;
    pos.x -= frame.origin.x * 4.f;
    pos.y -= frame.origin.y * 4.f;

    // TODO SetPosition should really only take a vec2.  Logically I should
    //      separate the location from the depth.
    vops::SetPosition(sprite->vertices, pos, frame.size * 4.f);
    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}

math::vec2
Player::GetWorldCenter (void) const noexcept
{
    return hitbox->GetWorldCenter();
}
