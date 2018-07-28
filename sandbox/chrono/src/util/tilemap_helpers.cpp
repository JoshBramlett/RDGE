#include <chrono/util/tilemap_helpers.hpp>

#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

using namespace rdge;

spawn_point_data
ProcessSpawnPoint (const tilemap::Object& obj)
{
    // Spawn points should never be nested objects, meaning the 'ext' data
    // should always be available
    SDL_assert(obj.ext_type == "spawn_point");
    SDL_assert(obj.ext_data);
    SDL_assert(obj.type == tilemap::ObjectType::POINT);

    const auto& props = obj.properties;
    const auto& ext_props = obj.ext_data->properties;

    spawn_point_data result;
    result.pos = obj.GetPoint();
    result.actor_id = (props.HasProperty("actor_id"))
        ? static_cast<chrono_actor_id>(props.GetInt("actor_id"))
        : static_cast<chrono_actor_id>(ext_props.GetInt("actor_id"));
    result.action_id = (props.HasProperty("action_id"))
        ? static_cast<chrono_action_id>(props.GetInt("action_id"))
        : static_cast<chrono_action_id>(ext_props.GetInt("action_id"));
    result.is_default = (props.HasProperty("is_default"))
        ? props.GetBool("is_default")
        : ext_props.GetBool("is_default");

    std::string facing = (props.HasProperty("facing"))
        ? props.GetString("facing")
        : ext_props.GetString("facing");

    bool parse_result = rdge::try_parse(facing, result.facing);
    SDL_assert(parse_result);

    return result;
}

action_trigger_data
ProcessActionTrigger (rdge::physics::RigidBody* body,
                      const tilemap::Object& obj,
                      const rdge::tilemap::extended_object_data* ext_data)
{
    // Action triggers may or may not be nested objects, so if the 'ext' data
    // is not available we retrieve it from the parent
    SDL_assert(obj.ext_type == "action_trigger");
    SDL_assert(obj.IsFixture());

    SDL_assert((ext_data && !obj.ext_data) || (!ext_data && obj.ext_data));
    if (!ext_data)
    {
        ext_data = obj.ext_data;
    }

    const auto& props = obj.properties;
    const auto& ext_props = ext_data->properties;

    action_trigger_data result;
    result.fixture = nullptr;
    result.action_id = (props.HasProperty("action_id"))
        ? static_cast<chrono_action_id>(props.GetInt("action_id"))
        : static_cast<chrono_action_id>(ext_props.GetInt("action_id"));
    result.scene_id = (props.HasProperty("scene_id"))
        ? static_cast<chrono_scene_id>(props.GetInt("scene_id"))
        : static_cast<chrono_scene_id>(ext_props.GetInt("scene_id"));
    result.invoke_required = (props.HasProperty("invoke_required"))
        ? props.GetBool("invoke_required")
        : ext_props.GetBool("invoke_required");

    physics::fixture_profile fprof;
    fprof.is_sensor = true;
    fprof.filter.category = chrono_collision_category_environment_triggers;
    fprof.filter.mask = chrono_collision_category_player_sensor_directional;
    // debug
    fprof.override_color = true;
    fprof.wireframe = ext_data->color;

    if (obj.type == tilemap::ObjectType::CIRCLE)
    {
        auto c = obj.GetCircle(g_game.ratios.base_to_world);
        fprof.shape = &c;
        result.fixture = body->CreateFixture(fprof);
    }
    else if (obj.type == tilemap::ObjectType::POLYGON)
    {
        // if the object has a parent tilemap placement is global
        auto p = obj.GetPolygon(g_game.ratios.base_to_world, (obj.parent != nullptr));
        fprof.shape = &p;
        result.fixture = body->CreateFixture(fprof);
    }
    else
    {
        SDL_assert(false);
    }

    return result;
}

physics::Fixture*
ProcessCollidable (rdge::physics::RigidBody* body,
                   const tilemap::Object& obj,
                   const rdge::tilemap::extended_object_data* ext_data)
{
    // Collidable objects may or may not be nested objects, so if the 'ext' data
    // is not available we retrieve it from the parent
    SDL_assert(obj.ext_type == "collidable");
    SDL_assert(obj.IsFixture());

    SDL_assert((ext_data && !obj.ext_data) || (!ext_data && obj.ext_data));
    if (!ext_data)
    {
        ext_data = obj.ext_data;
    }

    const auto& props = obj.properties;
    const auto& ext_props = ext_data->properties;

    physics::fixture_profile fprof;
    fprof.is_sensor = false;
    fprof.density = (props.HasProperty("density"))
        ? props.GetFloat("density")
        : ext_props.GetFloat("density");
    fprof.friction = (props.HasProperty("friction"))
        ? props.GetFloat("friction")
        : ext_props.GetFloat("friction");
    fprof.restitution = (props.HasProperty("restitution"))
        ? props.GetFloat("restitution")
        : ext_props.GetFloat("restitution");
    fprof.filter.category = (props.HasProperty("cgroup"))
        ? props.GetInt("cgroup")
        : ext_props.GetInt("cgroup");
    fprof.filter.mask = (props.HasProperty("cmask"))
        ? props.GetInt("cmask")
        : ext_props.GetInt("cmask");
    // debug
    fprof.override_color = true;
    fprof.wireframe = ext_data->color;

    physics::Fixture* result = nullptr;
    if (obj.type == tilemap::ObjectType::CIRCLE)
    {
        auto c = obj.GetCircle(g_game.ratios.base_to_world);
        fprof.shape = &c;
        result = body->CreateFixture(fprof);
    }
    else if (obj.type == tilemap::ObjectType::POLYGON)
    {
        // if the object has a parent tilemap placement is global
        auto p = obj.GetPolygon(g_game.ratios.base_to_world, (obj.parent != nullptr));
        fprof.shape = &p;
        result = body->CreateFixture(fprof);
    }
    else
    {
        SDL_assert(false);
    }

    SDL_assert(result);
    return result;
}
