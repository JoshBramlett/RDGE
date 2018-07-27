#pragma once

#include <chrono/globals.hpp>

#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/types.hpp>

//!@{ Forward declarations
namespace rdge {
namespace tilemap {
class Object;
} // namespace tilemap
} // namespace rdge
//!@}

struct chrono_spawn_point
{
    rdge::math::vec2 pos;
    rdge::Direction facing;
    chrono_actor_id actor_id;
    chrono_action_id action_id;
    bool is_default;
};

chrono_spawn_point
ProcessSpawnPoint (const rdge::tilemap::Object&);
