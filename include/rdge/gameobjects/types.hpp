//! \headerfile <rdge/gameobjects/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/30/2015

#pragma once
// TODO remove file?

#include <rdge/core.hpp>

//! \namespace RDGE: Rainbow Drop Game Engine
namespace rdge {
namespace gameobjects {

//! \enum Basic direction values
enum class Direction : rdge::uint8
{
    None = 0,
    Up,
    Right,
    Down,
    Left
};

} // gameobjects
} // rdge
