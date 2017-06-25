//! \headerfile <rdge/physics/collision_graph.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/06/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/bvh.hpp>
#include <rdge/physics/contact.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/intrusive_list.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \class ContactFilter
//! \brief Abstract class defining a ruleset for fixture collision
class ContactFilter
{
public:
    virtual ~ContactFilter (void) noexcept = default;

    //! \brief Default fixture collision rule
    //! \details The filter group supercedes the mask and category attributes.
    //!          If the group is the same, fixtures will collide with a positive
    //!          group index and never collide with a negative group index.  If
    //!          there is no group, a check is done against the mask and category.
    //! \param [in] a First fixture
    //! \param [in] b Second fixture
    //! \returns True if contact calculations should be performed
    virtual bool ShouldCollide (Fixture* a, Fixture* b) const noexcept
    {
        const auto& fa = a->filter;
        const auto& fb = b->filter;

        if ((fa.group != 0) && (fa.group == fb.group))
        {
            return (fa.group > 0);
        }

        return (fa.mask & fb.category) && (fb.mask & fa.category);
    }
};

class GraphListener
{
public:
    virtual ~GraphListener (void) noexcept = default;

    virtual void OnContactStart (Contact*) { }
    virtual void OnContactEnd (Contact*) { }
    virtual void OnPreSolve (Contact*, const collision_manifold*) { }
    virtual void OnPostSolve (Contact*, const contact_impulse*) { }

    //! \brief Triggered during destruction of the parent \ref RigidBody
    virtual void OnDestroyed (Fixture*) { }
};

class CollisionGraph
{
public:
    explicit CollisionGraph (const math::vec2& g);
    ~CollisionGraph (void) noexcept;

    RigidBody* CreateBody (const rigid_body_profile& profile);
    void DestroyBody (RigidBody* body);

    void CreateContact (fixture_proxy* a, fixture_proxy* b);
    void DestroyContact (Contact* contact);


    void Step (float dt);

    bool IsLocked (void) const noexcept { return m_flags & LOCKED; }


private:

    friend class RigidBody;

    void PurgeContacts (void);

    int32 RegisterProxy (fixture_proxy* proxy);
    void UnregisterProxy (const fixture_proxy* proxy);
    void MoveProxy (const fixture_proxy* proxy, const math::vec2& displacement);
    void TouchProxy (const fixture_proxy* proxy);

public:
    SmallBlockAllocator block_allocator;

    math::vec2 gravity;

    intrusive_list<RigidBody> bodies;


    // TODO maybe make contact data private?
    intrusive_list<Contact> contacts;
    ContactFilter* custom_filter = nullptr;
    GraphListener* listener = nullptr;



private:

    BVHTree m_tree;
    std::vector<int32> m_dirtyProxies;

    enum StateFlags
    {
        LOCKED    = 0x0001,
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
