//! \headerfile <rdge/physics/contact.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/24/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class Contact;
class GraphListener;
class Fixture;
class RigidBody;

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
    explicit Contact (Fixture* a, Fixture* b);
    ~Contact (void) noexcept = default;

    Contact (const Contact&) = delete;
    Contact (Contact&&) = delete;
    Contact& operator= (const Contact&) = delete;
    Contact& operator= (Contact&&) = delete;

    bool IsTouching (void) const noexcept { return m_flags & TOUCHING; }
    bool IsEnabled (void) const noexcept { return m_flags & ENABLED; }

    void Update (GraphListener* listener);

    Fixture* fixture_a = nullptr;
    Fixture* fixture_b = nullptr;

    contact_edge edge_a;
    contact_edge edge_b;

    float friction = 0.f;
    float restitution = 0.f;

    collision_manifold manifold;

private:

    friend class CollisionGraph;

    enum StateFlags
    {
        ENABLED  = 0x0001,
        TOUCHING = 0x0002,

        ON_ISLAND = 0x0004
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
