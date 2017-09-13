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
#include <rdge/physics/joints/base_joint.hpp>
#include <rdge/physics/solver.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/intrusive_list.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
namespace debug {
class PhysicsWidget;
}
//!@}

namespace physics {

class RevoluteJoint;

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
    virtual void OnPreSolve (Contact*, const collision_manifold&) { }
    virtual void OnPostSolve (Contact*) { }

    //! \brief Triggered during destruction of the parent \ref RigidBody
    virtual void OnDestroyed (Fixture*) { }
};

//! \struct time_step
//! \brief Container for time step data
//! \details Maintained by the \ref CollisionGraph, and includes support for
//!          handling variable time steps.
struct time_step
{
    float dt_0 = 0.f;  //!< Last step elapsed time
    float inv_0 = 0.f; //!< Last step inverse elapsed time

    float dt = 0.f;    //!< Elapsed time
    float inv = 0.f;   //!< Inverse elapsed time
    float ratio = 0.f; //!< Ratio from the last step to the current
};

class CollisionGraph
{
public:
    explicit CollisionGraph (const math::vec2& g);
    ~CollisionGraph (void) noexcept;
    void ClearGraph (void) noexcept;

    RigidBody* CreateBody (const rigid_body_profile& profile);
    void DestroyBody (RigidBody* body);

    RevoluteJoint* CreateRevoluteJoint (RigidBody* a, RigidBody* b, math::vec2 anchor);
    void DestroyJoint (BaseJoint* joint);

    void DisableForceClearing (void) noexcept { m_flags &= ~CLEAR_FORCES; }
    void ClearForces (void) noexcept;

    void Step (float dt);

    bool IsLocked (void) const noexcept { return m_flags & LOCKED; }

    bool IsSleepPrevented (void) const noexcept { return m_flags & PREVENT_SLEEP; }
    void PreventSleep (void) noexcept { m_flags |= PREVENT_SLEEP; }
    void AllowSleep (void) noexcept { m_flags &= ~PREVENT_SLEEP; }

private:

    friend class RigidBody;

    void CreateContact (fixture_proxy* a, fixture_proxy* b);
    void DestroyContact (Contact* contact);
    void PurgeContacts (void);

    int32 RegisterProxy (fixture_proxy* proxy);
    void UnregisterProxy (const fixture_proxy* proxy);
    void MoveProxy (const fixture_proxy* proxy, const math::vec2& displacement);
    void TouchProxy (const fixture_proxy* proxy);

public:

    SmallBlockAllocator block_allocator;    //!< Allocator for all simulation

    ContactFilter* custom_filter = nullptr; //!< Fixture filtering
    GraphListener* listener = nullptr;      //!< Callback listener

private:

    friend class rdge::debug::PhysicsWidget;

    BVHTree m_tree;
    Solver m_solver;

    std::vector<int32> m_dirtyProxies;
    intrusive_list<RigidBody> m_bodies;
    intrusive_list<Contact> m_contacts;
    intrusive_list<BaseJoint> m_joints;

    time_step m_step;

    enum StateFlags
    {
        LOCKED        = 0x0001,
        CLEAR_FORCES  = 0x0002,
        PREVENT_SLEEP = 0x0004
    };

    uint16 m_flags = 0;

#ifdef RDGE_DEBUG
public:
    struct profiler
    {
        int64_t create_contacts = 0;
        int64_t purge_contacts = 0;
        int64_t solve = 0;
        int64_t synchronize = 0;
    } debug_profile;
#endif
};

} // namespace physics
} // namespace rdge
