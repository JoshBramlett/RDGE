#pragma once

#include <rdge/core.hpp>

namespace rdge {
class Game;
class PackFile;
}

enum chrono_actor_id
{
    chrono_actor_none = 0,
    chrono_actor_chrono = 1,
    chrono_actor_autumn = 2
};

enum chrono_scene_id
{
    chrono_scene_none = 0,
    chrono_scene_overworld = 1,
    chrono_scene_winery    = 2
};

enum chrono_action_id
{
    chrono_action_none = 0,
    chrono_action_overworld_winery_main = 1,
    chrono_action_overworld_winery_store = 2
};

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

//! \struct game_ratios
//! \details Different ratios and scales to convert the different ways the game
//!          is represented.  Types are broken down into:
//!            - base - External (from assets)
//!            - world - Physics world
//!            - screen - Rendering pipeline
struct game_ratios
{
    game_ratios (void) = default;

    game_ratios (float ppm, float scale, float highdpi)
        : ppm(ppm)
        , ppm_base_scale(scale)
        , ppm_highdpi_scale(highdpi)
    {
         this->base_to_world = (1.f / this->ppm);
         this->base_to_screen = this->ppm_base_scale * this->ppm_highdpi_scale;
         this->world_to_screen = this->ppm * this->base_to_screen;
         this->screen_to_world = (1.f / this->world_to_screen);
    }

    float ppm = 0.f;
    float ppm_base_scale = 0.f;
    float ppm_highdpi_scale = 0.f;

    float base_to_world = 0.f;
    float base_to_screen = 0.f;
    float world_to_screen = 0.f;
    float screen_to_world = 0.f;
};

struct custom_event_data
{
    rdge::uint32 push_scene;
    rdge::uint32 pop_scene;
};

struct chrono_globals
{
    rdge::Game* game;
    rdge::PackFile* pack;

    game_ratios ratios;
    custom_event_data custom_events;
};

extern chrono_globals g_game;
