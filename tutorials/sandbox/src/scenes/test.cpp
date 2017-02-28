#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

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
    if (event.Type() == EventType::KeyDown)
    {
        auto args = event.GetKeyboardEventArgs();
        switch (args.Key())
        {
            case KeyCode::J:
                this->running = true;
                break;
            case KeyCode::W:
                this->facing = PlayerFacing::Back;
                this->walking = true;
                break;
            case KeyCode::A:
                this->facing = PlayerFacing::Left;
                this->walking = true;
                break;
            case KeyCode::S:
                this->facing = PlayerFacing::Front;
                this->walking = true;
                break;
            case KeyCode::D:
                this->facing = PlayerFacing::Right;
                this->walking = true;
                break;
            default:
                break;
        }
    }
    else if (event.Type() == EventType::KeyUp)
    {
        auto args = event.GetKeyboardEventArgs();
        switch (args.Key())
        {
            case KeyCode::J:
                this->running = false;
                break;
            case KeyCode::W:
            case KeyCode::A:
            case KeyCode::S:
            case KeyCode::D:
                this->walking = false;
                break;
            default:
                break;
        }
    }

    if (this->walking)
    {
        if (this->running)
        {
            if (this->state_type != PlayerStateType::Running)
            {
                this->state_type = PlayerStateType::Running;
                this->current_state = this->states[2].get();
                this->current_state->is_dirty = true;
            }
        }
        else
        {
            if (this->state_type != PlayerStateType::Walking)
            {
                this->state_type = PlayerStateType::Walking;
                this->current_state = this->states[1].get();
                this->current_state->is_dirty = true;
            }
        }
    }
    else
    {
        if (this->state_type != PlayerStateType::Idle)
        {
            this->state_type = PlayerStateType::Idle;
            this->current_state = this->states[0].get();
            this->current_state->is_dirty = true;
        }
    }
}

void
Player::OnUpdate (uint32 ticks)
{
    vops::SetTexCoords(this->sprite->vertices,
                       this->current_state->GetFrame(*this, ticks).coords);
}

const texture_part&
idle_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(int)player.facing];

    if (this->is_dirty || anim.IsFinished())
    {
        anim.Reset();
        this->offset = 0;
        this->is_dirty = false;
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
    auto& anim = this->animations[(int)player.facing];

    return anim.GetFrame(ticks);
}

const texture_part&
running_state::GetFrame (const Player& player, uint32 ticks)
{
    auto& anim = this->animations[(int)player.facing];

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
TestScene::OnUpdate (uint32 ticks)
{
    player.OnUpdate(ticks);
}

void
TestScene::OnRender (void)
{
    camera.Update();
    render_target->SetProjection(camera.combined);

    background.Draw();
}
