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
    {
    this->states.emplace_back(std::make_unique<idle_state>());
    auto& idle = this->states.back();

    Animation front(25);
    front.frames.push_back(sheet["idle_front_1"]);
    front.frames.push_back(sheet["idle_front_2"]);
    front.frames.push_back(sheet["idle_front_3"]);
    front.frames.push_back(sheet["idle_front_3"]);
    front.frames.push_back(sheet["idle_front_2"]);
    front.frames.push_back(sheet["idle_front_1"]);

    Animation back(25);
    back.frames.push_back(sheet["idle_back"]);

    const texture_part side1 = sheet["idle_side_1"];
    const texture_part side2 = sheet["idle_side_2"];
    const texture_part side3 = sheet["idle_side_3"];

    Animation right(25);
    right.frames.push_back(side1);
    right.frames.push_back(side2);
    right.frames.push_back(side3);

    Animation left(25);
    left.frames.push_back(side1.flip_horizontal());
    left.frames.push_back(side2.flip_horizontal());
    left.frames.push_back(side3.flip_horizontal());

    idle->animations.push_back(front);
    idle->animations.push_back(right);
    idle->animations.push_back(back);
    idle->animations.push_back(left);
    }
    //////////////////
    // walking animation
    //////////////////
    {
    this->states.emplace_back(std::make_unique<walking_state>());
    auto& walking = this->states.back();

    Animation front(Animation::PlayMode::Loop, 175);
    front.frames.push_back(sheet["walk_front_1"]);
    front.frames.push_back(sheet["walk_front_2"]);
    front.frames.push_back(sheet["walk_front_3"]);
    front.frames.push_back(sheet["walk_front_4"]);
    front.frames.push_back(sheet["walk_front_5"]);
    front.frames.push_back(sheet["walk_front_6"]);

    Animation back(Animation::PlayMode::Loop, 175);
    back.frames.push_back(sheet["walk_back_1"]);
    back.frames.push_back(sheet["walk_back_2"]);
    back.frames.push_back(sheet["walk_back_3"]);
    back.frames.push_back(sheet["walk_back_4"]);
    back.frames.push_back(sheet["walk_back_5"]);
    back.frames.push_back(sheet["walk_back_6"]);

    const texture_part side1 = sheet["walk_side_1"];
    const texture_part side2 = sheet["walk_side_2"];
    const texture_part side3 = sheet["walk_side_3"];
    const texture_part side4 = sheet["walk_side_4"];
    const texture_part side5 = sheet["walk_side_5"];
    const texture_part side6 = sheet["walk_side_6"];

    Animation right(Animation::PlayMode::Loop, 175);
    right.frames.push_back(side1);
    right.frames.push_back(side2);
    right.frames.push_back(side3);
    right.frames.push_back(side4);
    right.frames.push_back(side5);
    right.frames.push_back(side6);

    Animation left(Animation::PlayMode::Loop, 175);
    left.frames.push_back(side1.flip_horizontal());
    left.frames.push_back(side2.flip_horizontal());
    left.frames.push_back(side3.flip_horizontal());
    left.frames.push_back(side4.flip_horizontal());
    left.frames.push_back(side5.flip_horizontal());
    left.frames.push_back(side6.flip_horizontal());

    walking->animations.push_back(front);
    walking->animations.push_back(right);
    walking->animations.push_back(back);
    walking->animations.push_back(left);
    }

    //////////////////
    // running animation
    //////////////////
    {
    this->states.emplace_back(std::make_unique<running_state>());
    auto& running = this->states.back();

    Animation front(Animation::PlayMode::Loop, 100);
    front.frames.push_back(sheet["run_front_2"]);
    front.frames.push_back(sheet["walk_front_2"]);
    front.frames.push_back(sheet["walk_front_3"]);
    front.frames.push_back(sheet["run_front_1"]);
    front.frames.push_back(sheet["walk_front_5"]);
    front.frames.push_back(sheet["walk_front_6"]);

    Animation back(Animation::PlayMode::Loop, 100);
    back.frames.push_back(sheet["run_back_2"]);
    back.frames.push_back(sheet["walk_back_2"]);
    back.frames.push_back(sheet["walk_back_3"]);
    back.frames.push_back(sheet["run_back_1"]);
    back.frames.push_back(sheet["walk_back_5"]);
    back.frames.push_back(sheet["walk_back_6"]);

    const texture_part side1 = sheet["run_side_2"];
    const texture_part side2 = sheet["walk_side_2"];
    const texture_part side3 = sheet["walk_side_3"];
    const texture_part side4 = sheet["run_side_1"];
    const texture_part side5 = sheet["walk_side_5"];
    const texture_part side6 = sheet["walk_side_6"];

    Animation right(Animation::PlayMode::Loop, 100);
    right.frames.push_back(side1);
    right.frames.push_back(side2);
    right.frames.push_back(side3);
    right.frames.push_back(side4);
    right.frames.push_back(side5);
    right.frames.push_back(side6);

    Animation left(Animation::PlayMode::Loop, 100);
    left.frames.push_back(side1.flip_horizontal());
    left.frames.push_back(side2.flip_horizontal());
    left.frames.push_back(side3.flip_horizontal());
    left.frames.push_back(side4.flip_horizontal());
    left.frames.push_back(side5.flip_horizontal());
    left.frames.push_back(side6.flip_horizontal());

    running->animations.push_back(front);
    running->animations.push_back(right);
    running->animations.push_back(back);
    running->animations.push_back(left);
    }

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
