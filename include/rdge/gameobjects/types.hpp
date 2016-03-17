//! \headerfile <rdge/gameobjects/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/30/2015
//! \bug

#pragma once

#include <rdge/types.hpp>

//! \namespace RDGE: Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \enum Basic direction values
enum class Direction : RDGE::UInt8
{
    None = 0,
    Up,
    Right,
    Down,
    Left
};

} // GameObjects
} // RDGE
