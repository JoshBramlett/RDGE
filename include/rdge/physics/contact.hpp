//! \headerfile <rdge/physics/contact.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/24/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/util/containers/nodeless_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class Contact;
class ContactListener;
class Fixture;
class RigidBody;

struct contact_impulse
{
    float normalImpulses[2]; // b2_maxManifoldPoints
    float tangentImpulses[2];
    size_t count;
};

//! \struct contact_edge
//! \brief Represents contact between two bodies
//! \details The bodies which have fixtures in contact represent nodes in a
//!          graph and the contact is the edge between them.  This is used
//!          when determining which bodies make up an island.
struct contact_edge : public nodeless_list_element<contact_edge>
{
    RigidBody* other = nullptr; //!< Body connected by the edge
    Contact* contact = nullptr; //!< Contact connecting the bodies
};

class Contact : public nodeless_list_element<Contact>
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

    void Update (ContactListener* listener);

    Fixture* fixture_a = nullptr;
    Fixture* fixture_b = nullptr;

    contact_edge edge_a;
    contact_edge edge_b;

    float friction = 0.f;
    float restitution = 0.f;

    collision_manifold manifold;

private:

    enum StateFlags
    {
        ENABLED  = 0x0001,
        TOUCHING = 0x0002,
    };

    uint16 m_flags;
};

class ContactFilter
{
public:
    virtual ~ContactFilter (void) noexcept = default;

    virtual bool ShouldCollide (Fixture* a, Fixture* b) const noexcept;
};

class ContactListener
{
public:
    virtual ~ContactListener (void) noexcept = default;

    virtual void BeginContact (Contact*) { }
    virtual void EndContact (Contact*) { }
    virtual void PreSolve (Contact*, const collision_manifold*) { }
    virtual void PostSolve (Contact*, const contact_impulse*) { }
};

} // namespace physics
} // namespace rdge
