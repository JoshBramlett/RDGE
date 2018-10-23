#include <chrono/entities/npcs/debutante.hpp>
#include <chrono/asset_table.hpp>
#include <chrono/globals.hpp>
#include <chrono/types.hpp>

#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>
#include <rdge/events/event.hpp>
#include <rdge/debug/assert.hpp>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

namespace {

// sprite animations
CardinalDirectionArray<Animation> s_idle;
CardinalDirectionArray<Animation> s_walk;

} // anonymous namespace

Debutante::Debutante (void)
{
    static bool once [[gnu::unused]] = [](void) {
        auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_npcs);
        float s = g_game.ratios.base_to_screen;

        s_idle[Direction::UP]    = sheet->GetAnimation(animation_npcs_debutante_01_idle_front, s);
        s_idle[Direction::RIGHT] = sheet->GetAnimation(animation_npcs_debutante_01_idle_right, s);
        s_idle[Direction::DOWN]  = sheet->GetAnimation(animation_npcs_debutante_01_idle_front, s);
        s_idle[Direction::LEFT]  = sheet->GetAnimation(animation_npcs_debutante_01_idle_left, s);

        s_walk[Direction::UP]    = sheet->GetAnimation(animation_npcs_debutante_01_walk_back, s);
        s_walk[Direction::RIGHT] = sheet->GetAnimation(animation_npcs_debutante_01_walk_right, s);
        s_walk[Direction::DOWN]  = sheet->GetAnimation(animation_npcs_debutante_01_walk_front, s);
        s_walk[Direction::LEFT]  = sheet->GetAnimation(animation_npcs_debutante_01_walk_left, s);
        return true;
    }();
}

void
Debutante::Init (const math::vec2& pos, SpriteLayer& layer, CollisionGraph& graph)
{
    auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_npcs);
    this->sprite = layer.AddSprite(pos,
                                   frame_npcs_debutante_01_idle_front_1,
                                   *sheet,
                                   g_game.ratios.base_to_screen);

    this->facing = Direction::SOUTH;
    m_currentAnimation = &s_idle[this->facing];

    rigid_body_profile bprof;
    bprof.type = RigidBodyType::KINEMATIC;
    bprof.position = pos * g_game.ratios.base_to_world;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    this->body = graph.CreateBody(bprof);

    {
        // character collision
        fixture_profile fprof;
        //fprof.density = 5.f;
        //fprof.restitution = 0.8f;
        fprof.filter.category = chrono_collision_category_npc;
        fprof.filter.mask = chrono_collision_category_environment_static |
                            chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_player_hitbox;

        polygon p(0.5f, 0.25f, math::vec2(0.f, -0.75f));
        fprof.shape = &p;
        this->envbox = body->CreateFixture(fprof);
    }
}

void
Debutante::OnEvent (const Event&)
{ }

void
Debutante::OnUpdate (const delta_time& dt)
{
    static Random rand;

    float velocity = 0.f;
    if ((m_flags & MOVING) == 0)
    {
        m_currentAnimation = &s_idle[this->facing];

        if (rand.Next(600) == 0) // approx every 10 seconds
        {
            m_flags |= MOVING;
            this->normal = rand.Normal();
            this->facing = GetDirection(this->normal);
        }
    }
    else
    {
        velocity = 2.5f;
        m_currentAnimation = &s_walk[this->facing];

        if (rand.Next(180) == 0) // approx every 3 seconds
        {
            m_flags &= ~MOVING;
        }
    }

    body->linear.velocity = (this->normal * velocity);

    auto& frame = this->m_currentAnimation->GetFrame(dt.ticks);
    math::vec2 screen_pos(this->body->GetWorldCenter() * g_game.ratios.world_to_screen);

    sprite->pos = screen_pos - frame.origin;
    sprite->size = frame.size;
    sprite->uvs = frame.uvs;
}

void
Debutante::OnActionTriggered (const fixture_user_data&)
{
    RDGE_ASSERT(false);
}

uint32
Debutante::GetActorId (void) const noexcept
{
    return 0;
}

math::vec2
Debutante::GetWorldCenter (void) const noexcept
{
    return this->body->GetWorldCenter();
}
