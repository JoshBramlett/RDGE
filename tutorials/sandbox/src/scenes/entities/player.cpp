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
    cd_anim_blink.animations.emplace_back(sheet.GetAnimation("idle_back"));
    cd_anim_blink.animations.emplace_back(sheet.GetAnimation("idle_right"));
    cd_anim_blink.animations.emplace_back(sheet.GetAnimation("idle_front"));
    cd_anim_blink.animations.emplace_back(sheet.GetAnimation("idle_left"));

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk.animations.emplace_back(sheet.GetAnimation("walk_back"));
    cd_anim_walk.animations.emplace_back(sheet.GetAnimation("walk_right"));
    cd_anim_walk.animations.emplace_back(sheet.GetAnimation("walk_front"));
    cd_anim_walk.animations.emplace_back(sheet.GetAnimation("walk_left"));

    //////////////////
    // running animation
    //////////////////
    cd_anim_run.animations.emplace_back(sheet.GetAnimation("run_back"));
    cd_anim_run.animations.emplace_back(sheet.GetAnimation("run_right"));
    cd_anim_run.animations.emplace_back(sheet.GetAnimation("run_front"));
    cd_anim_run.animations.emplace_back(sheet.GetAnimation("run_left"));

    //////////////////
    // sheathe animation
    //////////////////
    cd_anim_sheathe.animations.emplace_back(sheet.GetAnimation("sheathe_back"));
    cd_anim_sheathe.animations.emplace_back(sheet.GetAnimation("sheathe_right"));
    cd_anim_sheathe.animations.emplace_back(sheet.GetAnimation("sheathe_front"));
    cd_anim_sheathe.animations.emplace_back(sheet.GetAnimation("sheathe_left"));

    //////////////////
    // fight stance animation
    //////////////////
    cd_anim_fight.animations.emplace_back(sheet.GetAnimation("fight_stance_back"));
    cd_anim_fight.animations.emplace_back(sheet.GetAnimation("fight_stance_right"));
    cd_anim_fight.animations.emplace_back(sheet.GetAnimation("fight_stance_front"));
    cd_anim_fight.animations.emplace_back(sheet.GetAnimation("fight_stance_left"));

    this->sprite = sheet.CreateSprite("idle_front_1", vec3(0.f, 0.f, 0.f));
    this->sprite->debug_bounds.show = true;

    user_input.disp.unit_length = 64.f;
    user_input.disp.base_velocities.push_back(10.f);
    user_input.disp.base_velocities.push_back(20.f);

    user_input.facing = Direction::SOUTH;
    current_animation = &cd_anim_blink[user_input.facing];
}

void
Player::OnEvent (const Event& event)
{
    user_input.dir_handler.OnEvent(event);

    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating())
        {
            return; // ignore repeating events - nothing has changed
        }

        if (args.PhysicalKey() == ScanCode::J)
        {
            user_input.run_button_pressed = args.IsKeyPressed();
        }
        else if (args.PhysicalKey() == ScanCode::K)
        {
            user_input.sheathe_button_pressed = args.IsKeyPressed();
        }
        else if (args.PhysicalKey() == ScanCode::L)
        {
            user_input.fight_button_pressed = args.IsKeyPressed();
        }
    }
}

void
Player::OnUpdate (const delta_time& dt)
{
    user_input.calculate(dt);
    uint32 ticks = dt.ticks;
    if (!user_input.is_moving)
    {
        // TODO No blinking animation enabled - but the first frame is used for
        //      the oon-moving (idle) state.  To enable I need to set a counter
        //      for the time idle, and play the animation after the threshold has
        //      been met.  Also need to reset the animation if finished.
        //
        //      if (player.current_state_changed || anim.IsFinished())
        //      {
        //          anim.Reset();
        //          offset = 0;
        //      }
        //
        //      offset += ticks;
        //      if (offset < 4000)
        //      {
        //          ticks = 0;
        //      }
        if (user_input.sheathe_button_pressed)
        {
            current_animation = &cd_anim_sheathe[user_input.facing];
        }
        else if (user_input.fight_button_pressed)
        {
            current_animation = &cd_anim_fight[user_input.facing];
        }
        else
        {
            ticks = 0;
            current_animation = &cd_anim_blink[user_input.facing];
        }
    }
    else if (user_input.is_walking)
    {
        current_animation = &cd_anim_walk[user_input.facing];
    }
    else if (user_input.is_running)
    {
        current_animation = &cd_anim_run[user_input.facing];
    }

    // naive wall collision check
    //auto prect = vops::GetRect(this->sprite->vertices);
    //if (prect.left() + user_input.position_offset.x < -960.f ||
        //prect.right() + user_input.position_offset.x > 960.f ||
        //prect.bottom() + user_input.position_offset.y < -540.f ||
        //prect.top() + user_input.position_offset.y > 540.f)
    //{
        //user_input.position_offset = {0.f, 0.f};
    //}

    vops::UpdatePosition(this->sprite->vertices, user_input.position_offset);
    vops::SetTexCoords(this->sprite->vertices,
                       current_animation->GetFrame(ticks).coords);
}
