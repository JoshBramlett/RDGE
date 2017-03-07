#include "test.hpp"

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
    this->current_state = this->states[static_cast<uint32>(PlayerStateType::Idle)].get();
}

void
Player::OnEvent (const Event& event)
{
    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating())
        {
            // ignore repeating events - nothing has changed
            return;
        }

        this->displacement.set_key_state(args.Key(), args.IsKeyPressed());
    }

    // ignore non-keyboard events
}

void
Player::OnUpdate (const delta_time& dt)
{
    this->current_state_changed = false;

    if (this->displacement.is_dirty)
    {
        if (this->displacement.is_running() &&
            this->state_type != PlayerStateType::Running)
        {
            this->state_type = PlayerStateType::Running;
            this->current_state = this->states[(uint32)PlayerStateType::Running].get();
            this->current_state_changed = true;
        }
        else if (this->displacement.is_walking() &&
                 this->state_type != PlayerStateType::Walking)
        {
            this->state_type = PlayerStateType::Walking;
            this->current_state = this->states[(uint32)PlayerStateType::Walking].get();
            this->current_state_changed = true;
        }
        else if (!this->displacement.is_moving() &&
                 this->state_type != PlayerStateType::Idle)
        {
            this->state_type = PlayerStateType::Idle;
            this->current_state = this->states[(uint32)PlayerStateType::Idle].get();
            this->current_state_changed = true;
        }
    }

    this->displacement.calculate(dt);
    //this->displacement.calculate_with_acceleration(dt);
    vops::UpdatePosition(this->sprite->vertices, this->displacement.uvec);

    vops::SetTexCoords(this->sprite->vertices,
                       this->current_state->GetFrame(*this, dt.ticks).coords);
}

const texture_part&
idle_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(int)player.displacement.facing];
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
    auto& anim = this->animations[(uint32)player.displacement.facing];
    //if (player.current_state_changed)
    //{
        //anim.Reset();
    //}

    return anim.GetFrame(ticks);
}

const texture_part&
running_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(uint32)player.displacement.facing];
    //if (player.current_state_changed)
    //{
        //anim.Reset();
    //}

    return anim.GetFrame(ticks);
}

TestScene::TestScene (void)
    : render_target(std::make_shared<SpriteBatch>(10'000))
    , background(render_target)
{
    SpriteSheet sheet("res/environment.json", Window::Current().IsHighDPI());

    ///////////////////
    // Background layer
    ///////////////////

    background.AddSprite(sheet.CreateSpriteChain("dirt",
                                                 vec3(-960.f, -540.f, 0.f),
                                                 vec2(1920.f, 1080.f)));

    Random rng;
    for (float y = -540.f; y < 540.f; y += 128.f)
    {
        for (float x = -960.f; x < 960.f; x += 128.f)
        {
            auto random = rng.Next(15);
            if (random == 0)
            {
                background.AddSprite(sheet.CreateSprite("weed1", vec3(x, y, 0.f)));
            }
            else if (random == 1)
            {
                background.AddSprite(sheet.CreateSprite("rock1", vec3(x, y, 0.f)));
            }
        }
    }

    background.AddSprite(player.sprite);
}

void
TestScene::Initialize (void)
{ }

void
TestScene::Terminate (void)
{ }

void
TestScene::Hibernate (void)
{ }

void
TestScene::Activate (void)
{ }

void
TestScene::OnEvent (const Event& event)
{
    player.OnEvent(event);
}

void
TestScene::OnUpdate (const delta_time& dt)
{
    player.OnUpdate(dt);
}

void
TestScene::OnRender (void)
{
    camera.Update();
    render_target->SetProjection(camera.combined);

    background.Draw();
}
