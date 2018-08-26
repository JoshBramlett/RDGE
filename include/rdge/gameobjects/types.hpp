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

//! \enum ActionType
//! \brief Action to take when invoked
enum class ActionType
{
    NONE       = 0,
    DIALOG     = 1,
    SCENE_PUSH = 2,
    SCENE_POP  = 3,
    SCENE_SWAP = 4,

    COUNT      = 5
};

template <typename T>
class ActionTypeArray
{
public:
    T& operator[] (ActionType t) { return m_arr[to_underlying(t)]; }
    const T& operator[] (ActionType t) const { return m_arr[to_underlying(t)]; }

private:
    T m_arr[to_underlying(ActionType::COUNT)];
};

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
    T& operator[] (Direction d) { return m_arr[math::lsb(to_underlying(d)) - 1]; }
    const T& operator[] (Direction d) const { return m_arr[math::lsb(to_underlying(d)) - 1]; }

private:
    T m_arr[4];
};

//! \brief ActionType stream output operator
std::ostream& operator<< (std::ostream&, ActionType);

//! \brief Direction stream output operator
std::ostream& operator<< (std::ostream&, Direction);

//!@{ ActionType string conversions
bool try_parse (const std::string&, ActionType&);
std::string to_string (ActionType);
//!@}

//!@{ Direction string conversions
bool try_parse (const std::string&, Direction&);
std::string to_string (Direction);
//!@}

} // namespace rdge
