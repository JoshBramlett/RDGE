#include <chrono/entities/static_actor.hpp>
#include <chrono/asset_table.hpp>
#include <chrono/globals.hpp>
#include <chrono/import.hpp>

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
        // Note: This is slightly confusing:
        //
        // As defined in the Tiled app, sprite objects can contain child objects
        // which generally represent the collidable regions.  The child objects
        // (Fixtures) have a relative position to the parent object (RigidBody).
        //
        // Therefore, in order to get correct positioning we have to use the position
        // of the sprite for the parent because the sprite creation may update the
        // positioning based on trimming.  The trimming does not impact the child
        // placement so we can simply use the positioning as defined.
        //
        // Due to this discrepency, the parent and child positioning uses
        // different ratios.  The parent uses the screen_to_world ratio as the
        // position is from the sprite, and the children use the base_to_world
        // ratio as the position is from the definition.

        rigid_body_profile bprof;
        bprof.type = RigidBodyType::STATIC;
        bprof.position = sprite->pos * g_game.ratios.screen_to_world;
        this->body = graph.CreateBody(bprof);

        // cache lookup, even if we don't need it
        auto ext_data_a = obj.parent->GetSharedObjectData("collidable");
        auto ext_data_b = obj.parent->GetSharedObjectData("action_trigger");
        SDL_assert(ext_data_a);
        SDL_assert(ext_data_b);

        for (size_t i = 0; i < region.objects.size(); i++)
        {
            const auto& child = region.objects[i];
            if (child.ext_type == "collidable")
            {
                this->collidables.emplace_back(perch::ProcessCollidable(body, child, ext_data_a));
            }
            else if (child.ext_type == "action_trigger")
            {
                this->triggers.emplace_back(perch::ProcessActionTrigger(body, child, ext_data_b));
            }
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

uint32
StaticActor::GetActorId (void) const noexcept
{
    return m_actorId;
}

math::vec2
StaticActor::GetWorldCenter (void) const noexcept
{
    return this->body->GetWorldCenter();
}
