//! \headerfile <rdge/physics/contact.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/24/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SmallBlockAllocator;
//!@}

namespace physics {

//!@{ Forward declarations
class Contact;
class CollisionGraph;
class GraphListener;
class Solver;
class RigidBody;
class Fixture;
//!@}

//! \struct contact_edge
//! \brief Represents contact between two bodies
//! \details The bodies which have fixtures in contact represent nodes in a
//!          graph and the contact is the edge between them.  This is used
//!          when determining which bodies make up an island.
struct contact_edge : public intrusive_list_element<contact_edge>
{
    RigidBody* other = nullptr; //!< Body connected by the edge
    Contact* contact = nullptr; //!< Contact connecting the bodies
};

class Contact : public intrusive_list_element<Contact>
{
public:
    //!@{ Non-copyable, Non-movable
    Contact (const Contact&) = delete;
    Contact& operator= (const Contact&) = delete;
    Contact (Contact&&) = delete;
    Contact& operator= (Contact&&) = delete;
    //!@}

    bool IsTouching (void) const noexcept { return m_flags & TOUCHING; }
    bool IsEnabled (void) const noexcept { return m_flags & ENABLED; }
    bool HasSensor (void) const noexcept { return m_flags & HAS_SENSOR; }

    //!@{ \ref Fixture nodes linked by this contact
    Fixture* fixture_a = nullptr;
    Fixture* fixture_b = nullptr;
    //!@}

    //!@{ Pointers to edges stored by each \ref RigidBody
    contact_edge edge_a;
    contact_edge edge_b;
    //!@}

    float friction = 0.f;
    float restitution = 0.f;
    float tangent_speed = 0.f;

    collision_manifold manifold;
    contact_impulse impulse;

private:

    friend class CollisionGraph;
    friend class Solver;
    friend class rdge::SmallBlockAllocator;

    explicit Contact (Fixture* a, Fixture* b);
    ~Contact (void) noexcept = default;

    //! \brief Narrow phase contact evaluation
    //! \details Performs narrow phase intersection tests and manifold generation.
    //!          Responsible for sending contact listener events during state changes.
    void Update (GraphListener* listener);

    enum StateFlags
    {
        ENABLED    = 0x0001,
        TOUCHING   = 0x0002,
        HAS_SENSOR = 0x0004,
        ON_ISLAND  = 0x0008
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
