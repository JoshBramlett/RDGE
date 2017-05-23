//! \headerfile <rdge/physics/collision_graph.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/06/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class CollisionGraph
{
public:
    explicit CollisionGraph (const math::vec2& g)
        : gravity(g)
    { }

    ~CollisionGraph (void) noexcept
    {
        //RigidBody* body = bodies;
        //while (body)
        //{

        //}
    }

    RigidBody* CreateBody (const rigid_body_profile& profile);
    void DestroyBody (RigidBody* body);

    SmallBlockAllocator block_allocator;

    math::vec2 gravity;

    RigidBody* bodies = nullptr;
    size_t body_count = 0;
};

} // namespace physics
} // namespace rdge
