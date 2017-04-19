#include "duck.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

#include <rdge/physics/collision.hpp>

using namespace rdge;
using namespace rdge::math;

Duck::Duck (const math::vec3& position)
{
    SpriteSheet sheet("res/enemies.json", Window::Current().IsHighDPI());

    //////////////////
    // walking animation
    //////////////////
    cd_anim_walk[Direction::UP]    = sheet.GetAnimation("duck_back");
    cd_anim_walk[Direction::RIGHT] = sheet.GetAnimation("duck_right");
    cd_anim_walk[Direction::DOWN]  = sheet.GetAnimation("duck_front");
    cd_anim_walk[Direction::LEFT]  = sheet.GetAnimation("duck_left");

    this->sprite = sheet.CreateSprite("duck_front_1", position);
    this->sprite->debug_bounds.show = true;

    facing = Direction::SOUTH;
    current_animation = &cd_anim_walk[facing];
}

void
Duck::OnUpdate (const delta_time& dt)
{
    //vops::UpdatePosition(this->sprite->vertices, user_input.position_offset);
    vops::SetTexCoords(this->sprite->vertices,
                       current_animation->GetFrame(dt.ticks).coords);
}
