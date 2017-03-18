#include "player.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;

Player::Player (void)
{
    SpriteSheet sheet("res/player.json", Window::Current().IsHighDPI());

    //////////////////
    // idle animation
    //////////////////
    this->states.emplace_back(std::make_unique<idle_state>());
    auto& idle = this->states.back();

    idle->animations.emplace_back(sheet.GetAnimation("idle_front"));
    idle->animations.emplace_back(sheet.GetAnimation("idle_right"));
    idle->animations.emplace_back(sheet.GetAnimation("idle_back"));
    idle->animations.emplace_back(sheet.GetAnimation("idle_left"));

    //////////////////
    // walking animation
    //////////////////
    this->states.emplace_back(std::make_unique<walking_state>());
    auto& walking = this->states.back();

    walking->animations.emplace_back(sheet.GetAnimation("walk_front"));
    walking->animations.emplace_back(sheet.GetAnimation("walk_right"));
    walking->animations.emplace_back(sheet.GetAnimation("walk_back"));
    walking->animations.emplace_back(sheet.GetAnimation("walk_left"));

    //////////////////
    // running animation
    //////////////////
    this->states.emplace_back(std::make_unique<running_state>());
    auto& running = this->states.back();

    running->animations.emplace_back(sheet.GetAnimation("run_front"));
    running->animations.emplace_back(sheet.GetAnimation("run_right"));
    running->animations.emplace_back(sheet.GetAnimation("run_back"));
    running->animations.emplace_back(sheet.GetAnimation("run_left"));

    this->sprite = sheet.CreateSprite("idle_front_1", vec3(0.f, 0.f, 0.f));
    this->sprite->debug_bounds.show = true;

    this->current_state = this->states[static_cast<uint32>(PlayerStateType::Idle)].get();

    this->vdisp.unit_length = 64.f;
    this->vdisp.base_velocities.push_back(10.f);
    this->vdisp.base_velocities.push_back(20.f);

    this->adisp.unit_length = 64.f;
    this->adisp.base_velocities.push_back(10.f);
    this->adisp.base_velocities.push_back(20.f);
}

void
Player::OnEvent (const Event& event)
{
    dir_handler.OnEvent(event);

    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating())
        {
            return; // ignore repeating events - nothing has changed
        }

        if (args.PhysicalKey() == ScanCode::J)
        {
            run_pressed = args.IsKeyPressed();
        }
    }
}

void
Player::OnUpdate (const delta_time& dt)
{
    this->current_state_changed = false;

    auto result_pair = dir_handler.Calculate();
    auto& uvec = result_pair.first;

    bool is_moving = uvec.x != 0.f || uvec.y != 0.f;
    bool is_running = is_moving && run_pressed;
    bool is_walking = is_moving && !run_pressed;

    // displacement using acceleration
    //this->adisp.calculate(uvec, dt, (is_running) ? 1 : 0);

    // displacement using velocity
    if (is_moving)
    {
        this->vdisp.calculate(uvec, dt, (is_running) ? 1 : 0);
    }

    if (is_running && this->state_type != PlayerStateType::Running)
    {
        this->state_type = PlayerStateType::Running;
        this->current_state = this->states[(uint32)PlayerStateType::Running].get();
        this->current_state_changed = true;
    }
    else if (is_walking && this->state_type != PlayerStateType::Walking)
    {
        this->state_type = PlayerStateType::Walking;
        this->current_state = this->states[(uint32)PlayerStateType::Walking].get();
        this->current_state_changed = true;
    }
    else if (!is_moving && this->state_type != PlayerStateType::Idle)
    {
        this->state_type = PlayerStateType::Idle;
        this->current_state = this->states[(uint32)PlayerStateType::Idle].get();
        this->current_state_changed = true;
    }

    switch (result_pair.second)
    {
    case Direction::NORTH:
        facing = PlayerFacing::Back;
        break;
    case Direction::EAST:
        facing = PlayerFacing::Right;
        break;
    case Direction::SOUTH:
        facing = PlayerFacing::Front;
        break;
    case Direction::WEST:
        facing = PlayerFacing::Left;
        break;
    default:
        facing = PlayerFacing::Front;
        break;
    }

    vops::UpdatePosition(this->sprite->vertices, uvec);
    vops::SetTexCoords(this->sprite->vertices,
                       this->current_state->GetFrame(*this, dt.ticks).coords);
}

const texture_part&
idle_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(int)player.facing];
    if (player.current_state_changed || anim.IsFinished())
    {
        anim.Reset();
        this->offset = 0;
    }

    this->offset += ticks;
    if (this->offset < 4000)
    {
        ticks = 0;
    }

    return anim.GetFrame(ticks);
}

const texture_part&
walking_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(uint32)player.facing];

    return anim.GetFrame(ticks);
}

const texture_part&
running_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(uint32)player.facing];

    return anim.GetFrame(ticks);
}
