#include <chrono/util/tilemap_helpers.hpp>

#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;
using namespace rdge::tilemap;

chrono_spawn_point
ProcessSpawnPoint (const Object& obj)
{
    SDL_assert(obj.ext_type == "actor_marker");
    SDL_assert(obj.ext_data);
    SDL_assert(obj.type == tilemap::ObjectType::POINT);

    const auto& props = obj.properties;
    const auto& ext_props = obj.ext_data->properties;

    // override from shared data if available
    chrono_spawn_point result;
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
