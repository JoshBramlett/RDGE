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

// image pixels/meter
float base_asset_ppm = 16.f;

// sprite animations
CardinalDirectionArray<Animation> s_fly;

} // anonymous namespace

Dove::Dove (void)
{
    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_spritesheet_enemies);

    static bool once [[gnu::unused]] = [&sheet](void) {
        float scale = g_game.ppm / base_asset_ppm;
        s_fly[Direction::RIGHT] = sheet.GetAnimation(enemies_animation_dove_right, scale);
        s_fly[Direction::LEFT]  = sheet.GetAnimation(enemies_animation_dove_left, scale);
        return true;
    }();

    this->sprite = std::make_shared<Sprite>(vec3(), vec2(), sheet.texture);
    this->facing = Direction::LEFT;
    m_currentAnimation = &s_fly[this->facing];
}

void
Dove::InitPhysics (CollisionGraph& graph, const math::vec2& pos)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.position = pos;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.simulate = false;
    this->body = graph.CreateBody(bprof);

    {
        // body
        fixture_profile fprof;
        fprof.filter.group = -1;
        fprof.filter.category = chrono_collision_category_none;
        fprof.filter.mask = chrono_collision_category_none;

        auto c = circle(0.5f);
        fprof.shape = &c;
        body->CreateFixture(fprof);
    }
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

        const auto& frame = m_currentAnimation->GetFrame(dt.ticks);
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
