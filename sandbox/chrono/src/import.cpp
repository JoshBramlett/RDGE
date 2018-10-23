#include <chrono/import.hpp>
#include <chrono/globals.hpp>
#include <chrono/types.hpp>
#include <chrono/entities/iactor.hpp>
#include <chrono/entities/static_actor.hpp>

#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/physics.hpp>
#include <rdge/debug/assert.hpp>

using namespace rdge;

namespace perch {

spawn_point_data
ProcessSpawnPoint (const tilemap::Object& obj)
{
    // Spawn points should never be nested objects, meaning the 'ext' data
    // should always be available
    RDGE_ASSERT(obj.ext_type == "spawn_point");
    RDGE_ASSERT(obj.ext_data);
    RDGE_ASSERT(obj.type == tilemap::ObjectType::POINT);

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

    {
        std::string s = (props.HasProperty("facing"))
            ? props.GetString("facing")
            : ext_props.GetString("facing");

        bool parse_result = rdge::try_parse(s, result.facing);
        RDGE_ASSERT(parse_result);
    }

    return result;
}

fixture_user_data
ProcessActionTrigger (rdge::physics::RigidBody* body,
                      const tilemap::Object& obj,
                      const rdge::tilemap::extended_object_data* ext_data)
{
    // Action triggers may or may not be nested objects, so if the 'ext' data
    // is not available we retrieve it from the parent
    RDGE_ASSERT(obj.ext_type == "action_trigger");
    RDGE_ASSERT(obj.IsFixture());

    RDGE_ASSERT((ext_data && !obj.ext_data) || (!ext_data && obj.ext_data));
    if (!ext_data)
    {
        ext_data = obj.ext_data;
    }

    const auto& props = obj.properties;
    const auto& ext_props = ext_data->properties;

    fixture_user_data result;
    result.type = fixture_user_data_action_trigger;
    result.fixture = nullptr;
    result.action_trigger.action_id = (props.HasProperty("action_id"))
        ? static_cast<chrono_action_id>(props.GetInt("action_id"))
        : static_cast<chrono_action_id>(ext_props.GetInt("action_id"));
    result.action_trigger.scene_id = (props.HasProperty("scene_id"))
        ? static_cast<chrono_scene_id>(props.GetInt("scene_id"))
        : static_cast<chrono_scene_id>(ext_props.GetInt("scene_id"));
    result.action_trigger.invoke_required = (props.HasProperty("invoke_required"))
        ? props.GetBool("invoke_required")
        : ext_props.GetBool("invoke_required");

    {
        std::string s = (props.HasProperty("action_type"))
            ? props.GetString("action_type")
            : ext_props.GetString("action_type");

        bool parse_result = perch::try_parse(s, result.action_trigger.action_type);
        RDGE_ASSERT(parse_result);
    }

    {
        std::string s = (props.HasProperty("facing_required"))
            ? props.GetString("facing_required")
            : ext_props.GetString("facing_required");

        bool parse_result = rdge::try_parse(s, result.action_trigger.facing_required);
        RDGE_ASSERT(parse_result);
    }

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
        RDGE_ASSERT(false);
    }

    return result;
}

fixture_user_data
ProcessCollidable (rdge::physics::RigidBody* body,
                   const tilemap::Object& obj,
                   const rdge::tilemap::extended_object_data* ext_data)
{
    // Collidable objects may or may not be nested objects, so if the 'ext' data
    // is not available we retrieve it from the parent
    RDGE_ASSERT(obj.ext_type == "collidable");
    RDGE_ASSERT(obj.IsFixture());

    RDGE_ASSERT((ext_data && !obj.ext_data) || (!ext_data && obj.ext_data));
    if (!ext_data)
    {
        ext_data = obj.ext_data;
    }

    const auto& props = obj.properties;
    const auto& ext_props = ext_data->properties;

    fixture_user_data result;
    result.type = fixture_user_data_collidable;
    result.fixture = nullptr;

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
        RDGE_ASSERT(false);
    }

    RDGE_ASSERT(result.fixture);
    return result;
}

void
ImportSpriteLayer (const tilemap::Layer& layer_def,
                   SpriteLayer& layer,
                   physics::CollisionGraph& graph,
                   std::vector<std::unique_ptr<IActor>>& actors)
{
    RDGE_ASSERT(layer_def.type == tilemap::LayerType::OBJECTGROUP);

    const auto& sheet = *layer_def.objectgroup.spritesheet;
    for (const auto& obj_def : layer_def.objectgroup.objects)
    {
        RDGE_ASSERT(obj_def.type == tilemap::ObjectType::SPRITE);
        actors.emplace_back(ImportTileObject(obj_def, sheet, layer, graph));
    }
}

std::unique_ptr<IActor>
ImportTileObject (const tilemap::Object& obj_def,
                  const SpriteSheet& sheet,
                  SpriteLayer& layer,
                  physics::CollisionGraph& graph)
{
    const auto& region = sheet.regions[obj_def.sprite.gid];
    ActorType actor_type = ActorType::NONE;
    if (!perch::try_parse(region.type, actor_type))
    {
        RDGE_ASSERT(false);
        return nullptr;
    }

    switch (actor_type)
    {
    case ActorType::STATIC:
        return std::make_unique<StaticActor>(obj_def, sheet, layer, graph);
    case ActorType::SIGN:
        return std::make_unique<StaticActor>(obj_def, sheet, layer, graph);
        //return std::make_unique<SignActor>();
    //case ActorType::CONTAINER:
        //return std::make_unique<WineryScene>();
    default:
        break;
    }

    RDGE_ASSERT(false);
    return nullptr;
}

} // namespace perch
