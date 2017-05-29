//! \headerfile <rdge/physics/collision_graph.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/06/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/contact.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/nodeless_list.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class CollisionGraph
{
public:
    explicit CollisionGraph (const math::vec2& g);
    ~CollisionGraph (void) noexcept;

    RigidBody* CreateBody (const rigid_body_profile& profile);
    void DestroyBody (RigidBody* body);

    void CreateContact (Fixture* a, Fixture* b);
    void DestroyContact (Contact* contact);
    void PurgeContacts (void);

    void RegisterProxy (fixture_proxy* proxy);
    void UnregisterProxy (fixture_proxy* proxy);

    void Step (float dt);

    bool IsLocked (void) const noexcept { return m_flags & LOCKED; }


    SmallBlockAllocator block_allocator;

    math::vec2 gravity;

    nodeless_list<RigidBody> bodies;


    // TODO maybe make contact data private?
    nodeless_list<Contact> contacts;
    ContactFilter* custom_filter = nullptr;
    ContactListener* listener = nullptr;



    struct step_iterations
    {
        uint8 velocity = 8;
        uint8 position = 3;
    } iterations;

private:
    enum StateFlags
    {
        LOCKED = 0x0001
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
