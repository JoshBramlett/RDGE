#include <chrono/entities/player.hpp>
#include <chrono/asset_table.hpp>
#include <chrono/globals.hpp>

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

namespace {

// sprite animations
CardinalDirectionArray<Animation> s_idle;
CardinalDirectionArray<Animation> s_walk;
CardinalDirectionArray<Animation> s_run;
CardinalDirectionArray<Animation> s_sheathe;
CardinalDirectionArray<Animation> s_fight;
CardinalDirectionArray<Animation> s_attack;

} // anonymous namespace

Player::Player (void)
{
    static bool once [[gnu::unused]] = [](void) {
        auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_player);
        float s = g_game.ratios.base_to_screen;

        s_idle[Direction::UP]    = sheet->GetAnimation(animation_player_idle_back, s);
        s_idle[Direction::RIGHT] = sheet->GetAnimation(animation_player_idle_right, s);
        s_idle[Direction::DOWN]  = sheet->GetAnimation(animation_player_idle_front, s);
        s_idle[Direction::LEFT]  = sheet->GetAnimation(animation_player_idle_left, s);

        s_walk[Direction::UP]    = sheet->GetAnimation(animation_player_walk_back, s);
        s_walk[Direction::RIGHT] = sheet->GetAnimation(animation_player_walk_right, s);
        s_walk[Direction::DOWN]  = sheet->GetAnimation(animation_player_walk_front, s);
        s_walk[Direction::LEFT]  = sheet->GetAnimation(animation_player_walk_left, s);

        s_run[Direction::UP]    = sheet->GetAnimation(animation_player_run_back, s);
        s_run[Direction::RIGHT] = sheet->GetAnimation(animation_player_run_right, s);
        s_run[Direction::DOWN]  = sheet->GetAnimation(animation_player_run_front, s);
        s_run[Direction::LEFT]  = sheet->GetAnimation(animation_player_run_left, s);

        s_sheathe[Direction::UP]    = sheet->GetAnimation(animation_player_sheathe_back, s);
        s_sheathe[Direction::RIGHT] = sheet->GetAnimation(animation_player_sheathe_right, s);
        s_sheathe[Direction::DOWN]  = sheet->GetAnimation(animation_player_sheathe_front, s);
        s_sheathe[Direction::LEFT]  = sheet->GetAnimation(animation_player_sheathe_left, s);

        s_fight[Direction::UP]    = sheet->GetAnimation(animation_player_fight_idle_back, s);
        s_fight[Direction::RIGHT] = sheet->GetAnimation(animation_player_fight_idle_right, s);
        s_fight[Direction::DOWN]  = sheet->GetAnimation(animation_player_fight_idle_front, s);
        s_fight[Direction::LEFT]  = sheet->GetAnimation(animation_player_fight_idle_left, s);

        s_attack[Direction::UP]    = sheet->GetAnimation(animation_player_attack_back, s);
        s_attack[Direction::RIGHT] = sheet->GetAnimation(animation_player_attack_right, s);
        s_attack[Direction::DOWN]  = sheet->GetAnimation(animation_player_attack_front, s);
        s_attack[Direction::LEFT]  = sheet->GetAnimation(animation_player_attack_left, s);
        return true;
    }();
}

void
Player::Init (const math::vec2& pos, SpriteLayer& layer, CollisionGraph& graph)
{
    auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_player);
    this->sprite = layer.AddSprite(pos,
                                   frame_player_idle_front_1,
                                   *sheet,
                                   g_game.ratios.base_to_screen);

    this->facing = Direction::SOUTH;
    m_currentAnimation = &s_idle[this->facing];

    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.position = pos * g_game.ratios.base_to_world;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    bprof.linear_damping = 0.5f;
    this->body = graph.CreateBody(bprof);

    {
        // hurtbox
        fixture_profile fprof;
        fprof.is_sensor = true;
        fprof.filter.category = chrono_collision_category_player_hitbox;
        fprof.filter.mask = chrono_collision_category_enemy_hitbox;

        polygon p(0.5f, 1.f);
        fprof.shape = &p;
        this->hurtbox = body->CreateFixture(fprof);
    }
    {
        // envbox (environment collidable region - not sure what else to call it)
        fixture_profile fprof;
        fprof.density = 1.f;
        //fprof.restitution = 0.8f;
        fprof.filter.category = chrono_collision_category_player_hitbox;
        fprof.filter.mask = chrono_collision_category_environment_static |
                            chrono_collision_category_npc;

        polygon p(0.5f, 0.25f, math::vec2(0.f, -0.75f));
        fprof.shape = &p;
        this->envbox = body->CreateFixture(fprof);
    }

#if 0
    {
        // directional sensors
        fixture_profile fprof;
        fprof.is_sensor = true;
        fprof.filter.category = chrono_collision_category_player_sensor_directional;
        fprof.filter.mask = chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_environment_triggers;

        circle c(math::vec2(-0.5f, 0.f), 0.95f);
        fprof.shape = &c;
        this->dir_sensors[Direction::LEFT] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.5f, 0.f), 0.95f);
        fprof.shape = &c;
        this->dir_sensors[Direction::RIGHT] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.f, 1.f), 0.5f);
        fprof.shape = &c;
        this->dir_sensors[Direction::UP] = body->CreateFixture(fprof);

        c = circle(math::vec2(0.f, -1.f), 0.5f);
        fprof.shape = &c;
        this->dir_sensors[Direction::DOWN] = body->CreateFixture(fprof);
    }
#endif
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
            SET_FLAG(args.IsKeyPressed(), m_flags, RUN_BUTTON_PRESSED);
        }

        if (args.PhysicalKey() == ScanCode::K)
        {
            SET_FLAG(args.IsKeyPressed(), m_flags, ATTACK_BUTTON_PRESSED);
        }
    }
}

void
Player::OnUpdate (const delta_time& dt)
{
    float velocity_scale = 0.f;

    if (m_flags & INPUT_LOCKED)
    {
        if (m_flags & ATTACKING)
        {
            velocity_scale = m_lockedVelocity;

            if (m_currentAnimation->IsFinished())
            {
                m_currentAnimation->Reset();
                m_flags &= ~(ATTACKING | INPUT_LOCKED);
            }
            else
            {
                Fixture* sensor = dir_sensors[this->facing];
                body->contact_edges.for_each([=](auto* edge) {
                    Contact* c = edge->contact;
                    Fixture* other = nullptr;
                    if (sensor == c->fixture_a)
                    {
                        other = c->fixture_b;
                    }
                    else if (sensor == c->fixture_b)
                    {
                        other = c->fixture_a;
                    }

                    if (other && c->IsTouching())
                    {
                        auto actor = static_cast<IActor*>(other->body->user_data);
                        actor->OnMeleeAttack(1.f, sensor->GetWorldCenter());
                    }
                });
            }
        }
    }

    if ((m_flags & INPUT_LOCKED) == 0)
    {
        auto p = m_handler.Calculate();
        this->normal = p.first;
        this->facing = p.second;

        if (m_flags & ATTACK_BUTTON_PRESSED)
        {
            BeginAttack();
        }
        else if (!this->normal.is_zero())
        {
            if (m_flags & RUN_BUTTON_PRESSED)
            {
                m_currentAnimation = &s_run[this->facing];
                velocity_scale = 12.5f * 2.f;
            }
            else
            {
                m_currentAnimation = &s_walk[this->facing];
                velocity_scale = 5.5f * 2.f;
            }
        }
        else
        {
            m_currentAnimation = &s_idle[this->facing];
        }
    }

#if 0
    // high damping if directional normal is different than the linear velocity
    body->linear.damping = (math::dot(this->normal, body->linear.velocity) > 0.f) ? 0.f : 9.f;

    math::vec2 delta = (this->normal * velocity_scale) - body->linear.velocity;
    math::vec2 impulse = delta * body->linear.mass;
    body->ApplyForce(impulse);
#else
    body->linear.velocity = (this->normal * velocity_scale);
#endif

    auto& frame = this->m_currentAnimation->GetFrame(dt.ticks);
    math::vec2 screen_pos(this->hurtbox->GetWorldCenter() * g_game.ratios.world_to_screen);

    sprite->pos = screen_pos - frame.origin;
    sprite->size = frame.size;
    sprite->uvs = frame.uvs;
}

void
Player::BeginAttack (void)
{
    m_currentAnimation = &s_attack[this->facing];
    m_flags |= (ATTACKING | INPUT_LOCKED);

    m_lockedVelocity = 0.f;
    if (!this->normal.is_zero())
    {
        if (m_flags & RUN_BUTTON_PRESSED)
        {
            m_lockedVelocity = 8.f;
        }
        else
        {
            m_lockedVelocity = 3.f;
        }
    }
}

void
Player::OnMeleeAttack (float damage, const rdge::math::vec2& pos)
{
    rdge::Unused(damage);
    rdge::Unused(pos);
}

math::vec2
Player::GetWorldCenter (void) const noexcept
{
    return hurtbox->GetWorldCenter();
}
