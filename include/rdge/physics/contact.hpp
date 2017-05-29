//! \headerfile <rdge/physics/contact.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/24/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/util/containers/nodeless_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class ContactListener;
class Fixture;
struct contact_edge;

struct contact_impulse
{
    float normalImpulses[2]; // b2_maxManifoldPoints
    float tangentImpulses[2];
    size_t count;
};

class Contact : public nodeless_list_element<Contact>
{
public:
    explicit Contact (Fixture* a, Fixture* b);
    ~Contact (void) noexcept;

    bool IsTouching (void) const noexcept { return m_flags & TOUCHING; }
    bool IsEnabled (void) const noexcept { return m_flags & ENABLED; }

    void Update (ContactListener* listener);

    Fixture* fixture_a = nullptr;
    Fixture* fixture_b = nullptr;

    contact_edge* edge_a = nullptr;
    contact_edge* edge_b = nullptr;

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
