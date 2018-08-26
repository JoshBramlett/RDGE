#pragma once

//!@{ Forward declarations
struct spawn_point_data;
struct fixture_user_data;
namespace rdge {
namespace tilemap {
class Object;
struct extended_object_data;
} // namespace tilemap
namespace physics {
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

namespace perch {

//! \brief Objects with ext_type == "spawn_point"
spawn_point_data
ProcessSpawnPoint (const rdge::tilemap::Object&);

//! \brief Objects with ext_type == "action_trigger"
fixture_user_data
ProcessActionTrigger (rdge::physics::RigidBody*,
                      const rdge::tilemap::Object&,
                      const rdge::tilemap::extended_object_data* = nullptr);

//! \brief Objects with ext_type == "collidable"
fixture_user_data
ProcessCollidable (rdge::physics::RigidBody*,
                   const rdge::tilemap::Object&,
                   const rdge::tilemap::extended_object_data* = nullptr);

} // namespace perch
