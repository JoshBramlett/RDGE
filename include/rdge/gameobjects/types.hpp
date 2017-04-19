//! \headerfile <rdge/gameobjects/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/16/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum Direction
//! \brief Bitmask representing cardinal and primary intercardinal directions
//! \note is_enum_bitmask is enabled and supports bitwise operations
enum class Direction : uint8
{
    NONE  = 0x00,

    NORTH = 0x01,
    EAST  = 0x02,
    SOUTH = 0x04,
    WEST  = 0x08,

    NE    = NORTH | EAST, // 0x03
    SE    = SOUTH | EAST, // 0x06
    SW    = SOUTH | WEST, // 0x0C
    NW    = NORTH | WEST, // 0x09

    UP    = NORTH,
    RIGHT = EAST,
    DOWN  = SOUTH,
    LEFT  = WEST,
};

//! \brief Enable Direction enum for bitmask operations
template<>
struct rdge::is_enum_bitmask<Direction> : public std::true_type { };

} // namespace rdge
