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

StaticActor::StaticActor (const Object& obj,
                          const SpriteSheet& sheet,
                          SpriteLayer& layer,
                          CollisionGraph& graph)
{
    this->sprite = layer.AddSprite(obj.pos,
                                   obj.sprite.gid,
                                   sheet,
                                   g_game.ratios.base_to_screen);

    const auto& region = sheet.regions[obj.sprite.gid];
    if (!region.objects.empty())
    {
        // The fixtures of a StaticActor are placed relative to the sprite
        // therefore we must use the position of the sprite (rather than the
        // object), as the AddSprite method will accommodate sprite trimming.

        rigid_body_profile bprof;
        bprof.type = RigidBodyType::STATIC;
        bprof.position = sprite->pos * g_game.ratios.screen_to_world;
        this->body = graph.CreateBody(bprof);

        // For now limit 4 fixtures per static actor
        SDL_assert(region.objects.size() <= MAX_FIXTURES);
        for (size_t i = 0; i < region.objects.size(); i++)
        {
            const auto& fixture_def = region.objects[i];

            // The region object data is just the fixture shape information,
            // therefore no parent (Tilemap) is available so the ext_data is
            // unpopulated.  The sprite object will have a parent, so we
            // query for the ext_data using the matching ext_type.
            SDL_assert(!fixture_def.ext_type.empty());
            auto ext_data = obj.parent->GetSharedObjectData(fixture_def.ext_type);
            SDL_assert(ext_data);

            const auto& ext_props = ext_data->properties;
            fixture_profile fprof;
            // debug
            fprof.override_color = true;
            fprof.wireframe = ext_data->color;
            // override from shared data if available
            fprof.density = (fixture_def.properties.HasProperty("density"))
                ? fixture_def.properties.GetFloat("density")
                : ext_props.GetFloat("density");
            fprof.friction = (fixture_def.properties.HasProperty("friction"))
                ? fixture_def.properties.GetFloat("friction")
                : ext_props.GetFloat("friction");
            fprof.restitution = (fixture_def.properties.HasProperty("restitution"))
                ? fixture_def.properties.GetFloat("restitution")
                : ext_props.GetFloat("restitution");
            fprof.is_sensor = (fixture_def.properties.HasProperty("is_sensor"))
                ? fixture_def.properties.GetBool("is_sensor")
                : ext_props.GetBool("is_sensor");
            fprof.filter.category = (fixture_def.properties.HasProperty("cgroup"))
                ? fixture_def.properties.GetInt("cgroup")
                : ext_props.GetInt("cgroup");
            fprof.filter.mask = (fixture_def.properties.HasProperty("cmask"))
                ? fixture_def.properties.GetInt("cmask")
                : ext_props.GetInt("cmask");

            if (fixture_def.type == tilemap::ObjectType::CIRCLE)
            {
                auto c = fixture_def.GetCircle(g_game.ratios.base_to_world);
                fprof.shape = &c;
                this->fixtures[i] = this->body->CreateFixture(fprof);
            }
            else if (fixture_def.type == tilemap::ObjectType::POLYGON)
            {
                auto p = fixture_def.GetPolygon(g_game.ratios.base_to_world);
                fprof.shape = &p;
                this->fixtures[i] = this->body->CreateFixture(fprof);
            }
            else
            {
                SDL_assert(false);
            }

            this->num_fixtures++;
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
    return this->body->GetWorldCenter();
}
