#pragma once

#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/types.hpp>

//!@{ Forward declarations
namespace rdge {
namespace physics {
class Fixture;
} // namespace physics
} // namespace rdge
//!@}


enum chrono_actor_id
{
    chrono_actor_none   = 0,
    chrono_actor_chrono = 1,
    chrono_actor_autumn = 2,
    // last
    chrono_actor_count  = 3
};

enum chrono_scene_id
{
    chrono_scene_none      = 0,
    chrono_scene_overworld = 1,
    chrono_scene_winery    = 2,
    // last
    chrono_scene_count     = 3
};

enum chrono_action_id
{
    chrono_action_none                   = 0,
    chrono_action_overworld_winery_main  = 1,
    chrono_action_overworld_winery_store = 2,
    // last
    chrono_action_count                  = 3
};

//! \enum chrono_collision_category
//! \detail Categories for determining which fixtures are able to collide with
//!         other fixtures.  Values can be stored in the tilemap definition as
//!         the bitmask integer value, so extreme care should be taken when
//!         modifying existing values.
enum chrono_collision_category
{
    chrono_collision_category_none                      = 0x0000,

    chrono_collision_category_reserved                  = 0x0001, //!< fixture default

    chrono_collision_category_environment_static        = 0x0002,
    chrono_collision_category_environment_triggers      = 0x0004,

    chrono_collision_category_enemy_hitbox              = 0x0010,

    chrono_collision_category_npc                       = 0x0100,

    chrono_collision_category_player_hitbox             = 0x1000,
    chrono_collision_category_player_sensor_directional = 0x2000,
    chrono_collision_category_player_sensor_proximity   = 0x4000,
    chrono_collision_category_player_sensor_spell       = 0x8000,

    chrono_collision_category_all                       = 0xFFFF,

    chrono_collision_category_all_hitbox = chrono_collision_category_enemy_hitbox |
                                           chrono_collision_category_player_hitbox
};

//! \struct spawn_point_data
//! \detail Represents where characters will spawn.  Often populated externally
//!         from the tilemap and a list is cached by the scenes.
struct spawn_point_data
{
    rdge::math::vec2 pos;
    rdge::Direction facing = rdge::Direction::NONE;
    chrono_action_id action_id = chrono_action_none;
    chrono_actor_id actor_id = chrono_actor_none;
    bool is_default = false;
};

//! \enum fixture_user_data_type
//! \detail Different fixture types determines what that fixture represents and
//!         what behavior is to occur on collision
enum fixture_user_data_type
{
    fixture_user_data_none                = 0x0000,

    fixture_user_data_collidable          = 0x0001,
    fixture_user_data_action_trigger      = 0x0002,

    fixture_user_data_player_sensor_left  = 0x1000,
    fixture_user_data_player_sensor_right = 0x2000,
    fixture_user_data_player_sensor_up    = 0x4000,
    fixture_user_data_player_sensor_down  = 0x8000,

    fixture_user_data_player_sensor_all   = fixture_user_data_player_sensor_left |
                                            fixture_user_data_player_sensor_right |
                                            fixture_user_data_player_sensor_up |
                                            fixture_user_data_player_sensor_down
};

//! \struct fixture_user_data
//! \detail Required type for physics fixtures user_data field.  Data is often
//!         populated from the tilemap
struct fixture_user_data
{
    fixture_user_data_type type = fixture_user_data_none;
    rdge::physics::Fixture* fixture = nullptr;

    struct action_trigger_data
    {
        rdge::ActionType action_type;
        chrono_action_id action_id;
        chrono_scene_id scene_id;
        rdge::Direction facing_required;
        bool invoke_required;
    } action_trigger;
};
