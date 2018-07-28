#pragma once

#include <chrono/globals.hpp>

#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/types.hpp>

//!@{ Forward declarations
namespace rdge {
namespace tilemap {
class Object;
struct extended_object_data;
} // namespace tilemap
namespace physics {
class Fixture;
class RigidBody;
} // namespace tilemap
} // namespace rdge
//!@}

// Note: This is for parsing the objects mapping to 'object_types' as defined
//       by the tilemap.  The convention is the shared 'ext' data represents
//       the default value.  Therefore, if the object itself contains a property
//       that is defined in the 'ext' data, that value from the object will
//       override the value defined in the shared data.
//
//       The objects from a tileset (aka objectsheet) will not have a
//       reference to the ext data because they don't have a tilemap parent.
//       These objects must provide a reference to the data when calling.

struct spawn_point_data
{
    rdge::math::vec2 pos;
    rdge::Direction facing = rdge::Direction::NONE;
    chrono_action_id action_id = chrono_action_none;
    chrono_actor_id actor_id = chrono_actor_none;
    bool is_default = false;
};

struct action_trigger_data
{
    rdge::physics::Fixture* fixture = nullptr;
    chrono_action_id action_id = chrono_action_none;
    chrono_scene_id scene_id = chrono_scene_none;
    bool invoke_required = false;
};

//! \brief Objects with ext_type == "spawn_point"
spawn_point_data
ProcessSpawnPoint (const rdge::tilemap::Object&);

//! \brief Objects with ext_type == "action_trigger"
action_trigger_data
ProcessActionTrigger (rdge::physics::RigidBody*,
                      const rdge::tilemap::Object&,
                      const rdge::tilemap::extended_object_data* = nullptr);

//! \brief Objects with ext_type == "collidable"
rdge::physics::Fixture*
ProcessCollidable (rdge::physics::RigidBody*,
                   const rdge::tilemap::Object&,
                   const rdge::tilemap::extended_object_data* = nullptr);
