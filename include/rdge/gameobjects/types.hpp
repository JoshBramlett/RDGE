//! \headerfile <rdge/gameobjects/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/16/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/math/vec2.hpp>

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

inline Direction
GetDirection (const math::vec2& ab)
{
    Direction result = Direction::NORTH;
    float best = math::dot(ab, math::vec2(0.f, 1.f));

    float len_east = math::dot(ab, math::vec2(1.f, 0.f));
    if (len_east > best)
    {
        best = len_east;
        result = Direction::EAST;
    }

    float len_south = math::dot(ab, math::vec2(0.f, -1.f));
    if (len_south > best)
    {
        best = len_south;
        result = Direction::SOUTH;
    }

    float len_west = math::dot(ab, math::vec2(-1.f, 0.f));
    if (len_west > best)
    {
        result = Direction::WEST;
    }

    return result;
}

template <typename T>
class CardinalDirectionArray
{
public:
    T& operator[] (rdge::Direction dir)
    {
        return elements[rdge::math::lsb(rdge::to_underlying(dir)) - 1];
    }

    T elements[4];
};

} // namespace rdge
