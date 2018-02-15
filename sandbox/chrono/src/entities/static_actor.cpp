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
                          const SpriteSheet& sheet,
                          SpriteLayer& layer,
                          CollisionGraph& graph)
{
    this->sprite = layer.AddSprite(def.pos,
                                   def.sprite.gid,
                                   sheet,
                                   g_game.ratios.base_to_screen);

    const auto& region = sheet.regions[def.sprite.gid];
    if (!region.objects.empty())
    {
        rigid_body_profile bprof;
        bprof.type = RigidBodyType::STATIC;
        bprof.position = sprite->pos * g_game.ratios.screen_to_world;
        this->body = graph.CreateBody(bprof);

        // For now assume 1 fixture per sprite region
        SDL_assert(region.objects.size() == 1);

        // TODO these properties should be read by the object
        fixture_profile fprof;
        fprof.density = 1.f;
        fprof.restitution = 0.8f;
        fprof.filter.category = chrono_collision_category_environment_static;
        fprof.filter.mask = chrono_collision_category_all_hitbox;

        const auto& shape = region.objects[0];
        if (shape.type == tilemap::ObjectType::CIRCLE)
        {
            auto c = shape.GetCircle(g_game.ratios.base_to_world);
            fprof.shape = &c;
            this->hitbox = this->body->CreateFixture(fprof);
        }
        if (shape.type == tilemap::ObjectType::POLYGON)
        {
            auto p = shape.GetPolygon(g_game.ratios.base_to_world);
            fprof.shape = &p;
            this->hitbox = this->body->CreateFixture(fprof);
        }
    }
}

void
StaticActor::OnEvent (const Event&)
{ }

void
StaticActor::OnUpdate (const delta_time&)
{
    // TODO collision graph culling
}

void
StaticActor::OnMeleeAttack (float, const math::vec2&)
{ }

math::vec2
StaticActor::GetWorldCenter (void) const noexcept
{
    return hitbox->GetWorldCenter();
}
