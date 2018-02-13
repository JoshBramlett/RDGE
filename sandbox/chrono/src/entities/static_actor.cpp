#include <chrono/entities/static_actor.hpp>
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
using namespace rdge::tilemap;

StaticActor::StaticActor (const Object& def,
                          SpriteLayer& layer,
                          CollisionGraph& graph)
{
    auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_player);
    this->sprite = layer.AddSprite(pos,
                                   frame_player_idle_front_1,
                                   *sheet,
                                   g_game.ratios.base_to_screen);

    this->facing = Direction::SOUTH;
    m_currentAnimation = &s_idle[this->facing];
}

void
StaticActor::InitPhysics (CollisionGraph& graph, const math::vec2& pos)
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.position = pos * g_game.ratios.base_to_world;
    bprof.gravity_scale = 0.f;
    bprof.prevent_rotation = true;
    bprof.prevent_sleep = true;
    bprof.linear_damping = 0.0f;
    this->body = graph.CreateBody(bprof);

    {
        // hitbox
        fixture_profile fprof;
        fprof.density = 1.f;
        fprof.restitution = 0.8f;
        fprof.filter.category = chrono_collision_category_player_hitbox;
        fprof.filter.mask = chrono_collision_category_enemy_hitbox |
                            chrono_collision_category_environment_static;

        polygon p(0.5f, 1.f);
        fprof.shape = &p;
        this->hitbox = body->CreateFixture(fprof);
    }

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
}

void
StaticActor::OnEvent (const Event& event)
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
StaticActor::OnUpdate (const delta_time& dt)
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
                velocity_scale = 12.5f;
            }
            else
            {
                m_currentAnimation = &s_walk[this->facing];
                velocity_scale = 5.5f;
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
    math::vec2 screen_pos(this->hitbox->GetWorldCenter() * g_game.ratios.world_to_screen);

    sprite->pos = screen_pos - frame.origin;
    sprite->size = frame.size;
    sprite->uvs = frame.uvs;
}

void
StaticActor::BeginAttack (void)
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
StaticActor::OnMeleeAttack (float damage, const rdge::math::vec2& pos)
{
    rdge::Unused(damage);
    rdge::Unused(pos);
}

math::vec2
StaticActor::GetWorldCenter (void) const noexcept
{
    return hitbox->GetWorldCenter();
}
