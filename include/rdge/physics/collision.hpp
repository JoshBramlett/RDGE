//! \headerfile <rdge/physics/collision.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

// sites of import:
//
// http://www.iforce2d.net/b2dtut/collision-anatomy
// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct collision_manifold
//! \brief Container for collision resolution details
struct collision_manifold
{
    uint32 count = 0;               //!< Number of collision points
    float depths[2] = { 0.f, 0.f }; //!< Penetration depths
    math::vec2 contacts[2];         //!< Contact points
    math::vec2 normal;              //!< Vector of resolution, or collision normal
};

} // namespace rdge
