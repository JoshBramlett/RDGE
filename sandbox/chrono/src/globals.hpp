#pragma once

namespace rdge {
class Game;
class PackFile;
}

enum chrono_collision_category
{
    chrono_collision_category_none                      = 0x0000,

    chrono_collision_category_player_hitbox             = 0x0001,
    chrono_collision_category_player_sensor_directional = 0x0002,
    chrono_collision_category_player_sensor_proximity   = 0x0004,
    chrono_collision_category_player_sensor_spell       = 0x0008,

    chrono_collision_category_enemy_hitbox              = 0x0010,

    chrono_collision_category_environment_static        = 0x0100,
    chrono_collision_category_environment_triggers      = 0x0200,
};

struct chrono_globals
{
    rdge::Game* game;
    rdge::PackFile* pack;

    float ppm;
    float inv_ppm;
};

extern chrono_globals g_game;
