#include "dove.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

#include "../../asset_enums.hpp"
#include "../../globals.hpp"

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

namespace {

    float base_asset_ppm = 16.f;

} // anonymous namespace

Dove::Dove (void)
{
    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_spritesheet_enemies);

    float scale = g_game.ppm / base_asset_ppm;
    this->anim_fly_left = sheet.GetAnimation(enemies_animation_dove_left, scale);
    this->anim_fly_right = sheet.GetAnimation(enemies_animation_dove_right, scale);
    this->current_animation = &this->anim_fly_left;

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(), sheet.texture);
}

void
Dove::InitPhysics (CollisionGraph& graph, const math::vec2& pos)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.position = pos;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    bprof.simulate = false;
    body = graph.CreateBody(bprof);

    auto p = circle(0.5f);

    fixture_profile fprof;
    fprof.shape = &p;
    fprof.density = 1.f;
    fprof.restitution = 0.9f;

    // collides with nothing
    fprof.filter.group = -1;
    fprof.filter.category = 0;
    fprof.filter.mask = 0;

    body->CreateFixture(fprof);
}

void
Dove::Spawn (rdge::math::vec2 pos)
{
    body->Enable();
    body->SetPosition(pos);
    is_flying = true;
}

void
Dove::Disable (void)
{
    body->Disable();
    is_flying = false;
}

void
Dove::OnUpdate (const delta_time& dt)
{
    if (is_flying)
    {
        math::vec2 desired_velocity(-5.f, 0.f);
        this->body->ApplyLinearImpulse(desired_velocity - this->body->linear.velocity);

        const auto& frame = this->current_animation->GetFrame(dt.ticks);
        math::vec2 pos((this->body->GetWorldCenter() * g_game.ppm) - frame.origin);

        vops::SetPosition(this->sprite->vertices, pos, frame.size);
        vops::SetTexCoords(this->sprite->vertices, frame.coords);
    }
}

math::vec2
Dove::GetWorldCenter (void) const noexcept
{
    return body->GetWorldCenter();
}
