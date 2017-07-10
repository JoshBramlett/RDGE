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

Player::Player (void)
{
    SpriteSheet sheet("res/player.json", Window::Current().IsHighDPI());

    //////////////////
    // idle animation
    //////////////////
    cd_anim_blink[Direction::UP]    = sheet.GetAnimation("idle_back");
    cd_anim_blink[Direction::RIGHT] = sheet.GetAnimation("idle_right");
    cd_anim_blink[Direction::DOWN]  = sheet.GetAnimation("idle_front");
    cd_anim_blink[Direction::LEFT]  = sheet.GetAnimation("idle_left");

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation("walk_back");
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation("walk_right");
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation("walk_front");
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation("walk_left");

    //////////////////
    // running animation
    //////////////////
    cd_anim_run[Direction::UP]    = sheet.GetAnimation("run_back");
    cd_anim_run[Direction::RIGHT] = sheet.GetAnimation("run_right");
    cd_anim_run[Direction::DOWN]  = sheet.GetAnimation("run_front");
    cd_anim_run[Direction::LEFT]  = sheet.GetAnimation("run_left");

    //////////////////
    // sheathe animation
    //////////////////
    cd_anim_sheathe[Direction::UP]    = sheet.GetAnimation("sheathe_back");
    cd_anim_sheathe[Direction::RIGHT] = sheet.GetAnimation("sheathe_right");
    cd_anim_sheathe[Direction::DOWN]  = sheet.GetAnimation("sheathe_front");
    cd_anim_sheathe[Direction::LEFT]  = sheet.GetAnimation("sheathe_left");

    //////////////////
    // fight stance animation
    //////////////////
    cd_anim_fight[Direction::UP]    = sheet.GetAnimation("fight_stance_back");
    cd_anim_fight[Direction::RIGHT] = sheet.GetAnimation("fight_stance_right");
    cd_anim_fight[Direction::DOWN]  = sheet.GetAnimation("fight_stance_front");
    cd_anim_fight[Direction::LEFT]  = sheet.GetAnimation("fight_stance_left");

    //////////////////
    // attack animation
    //////////////////
    cd_anim_attack[Direction::UP]    = sheet.GetAnimation("attack_back");
    cd_anim_attack[Direction::RIGHT] = sheet.GetAnimation("attack_right");
    cd_anim_attack[Direction::DOWN]  = sheet.GetAnimation("attack_front");
    cd_anim_attack[Direction::LEFT]  = sheet.GetAnimation("attack_left");

    this->sprite = sheet.CreateSprite("idle_front_1", vec3(-64.f, -96.f, 0.f));
    this->sprite->debug_bounds.show = true;

    m_facing = Direction::SOUTH;
    current_animation = &cd_anim_blink[m_facing];
}

void
Player::InitPhysics (CollisionGraph& graph, float inv_ratio)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    bprof.linear_damping = 0.5f;
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

    {
        auto p = m_handler.Calculate();
        m_direction = p.first;
        m_facing = p.second;
    }

    math::vec2 desired_velocity = m_direction;
    bool is_moving = !m_direction.is_zero();
    if (is_moving)
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
    else if (m_flags & ATTACK_BUTTON_PRESSED)
    {
        current_animation = &cd_anim_attack[m_facing];
        if (current_animation->IsFinished())
        {
            current_animation->Reset();
        }
    }
    else
    {
        ticks = 0;
        current_animation = &cd_anim_blink[m_facing];
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

    // !!! The only reason this works is b/c the body has a single fixture !!!
    //
    // Once more fixtures are added (e.g. circle sensor that represents when
    // an enemy "hears" the player) this will break.  We'll need to cache the
    // fixture, and transform the local centroid of the players shape with
    // the body transform.
    math::vec2 pos = body->GetWorldCenter() * 64.f;
    pos.x -= frame.origin.x;
    pos.y -= frame.origin.y;

    // TODO SetPosition should really only take a vec2.  Logically I should
    //      separate the location from the depth.
    vops::SetPosition(sprite->vertices, pos, static_cast<vec2>(frame.size));
    vops::SetTexCoords(this->sprite->vertices, frame.coords);
}

math::vec2
Player::GetWorldCenter (void) const noexcept
{
    return body->GetWorldCenter();
}
