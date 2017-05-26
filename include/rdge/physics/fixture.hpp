//! \headerfile <rdge/physics/fixture.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/nodeless_list.hpp>

#include <SDL_assert.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

// Box2D dependency tree:
// Fixture
//   Body
//   BroadPhase
//   World
//   ContactManager
//   ContactEdge
//   Contact
//   BlockAllocator

class RigidBody;
class Fixture;

//! \struct collision_filter
//! \brief Collision filtering data
struct collision_filter
{
    uint16 group    = 0;
    uint16 category = 0x0001; //!< Category the filter belongs to
    uint16 mask     = 0xFFFF; //!< Mask of other categories the object can collide with
};

//! \struct fixture_profile
//! \brief Profile used to constrct a fixture
struct fixture_profile
{
    ishape* shape = nullptr;   //!< Fixture underlying shape
    void* user_data = nullptr; //!< Custom opaque pointer

    float density = 0.f;       //!< Shape density in kg/m^2
    float friction = 0.2f;     //!< Normalized friction coefficient
    float restitution = 0.f;   //!< Normalized elasticity

    collision_filter filter;   //!< Collision filter
    bool is_sensor = false;    //!< If fixture generates a collision response
};

//! \struct fixture_proxy
//! \brief Container for the shape's wrapping aabb
//! \details Used for broad phase collision detection.
struct fixture_proxy
{
    aabb box;
    Fixture* fixture = nullptr;
    int32 proxy_id = 0;
};

class Fixture : public nodeless_forward_list_element<Fixture>
{
public:
    explicit Fixture (const fixture_profile& profile, RigidBody* parent);
    ~Fixture (void) noexcept;

    void CreateProxy (/* broad_phase, const iso_transform& xf */)
    {
        throw "not yet implemented";
        // Box2D:
        // 1) create one proxy per shape child
        // 2) compute the aabb
        // 3) register the proxy to the broad phase
    }

    void DestroyProxy (/* broad_phase */)
    {
        throw "not yet implemented";
        // Box2D:
        // 1) have broad phase destroy each proxy
        // 2) assign the proxy id to "null" constant
    }

    void Synchronize (/* broad_phase, const iso_transform& xf1, const iso_transform& xf2 */)
    {
        throw "not yet implemented";
        // This is used to update the aabbs in the proxies
        // The two transforms are used to combine the aabbs in the sweep step

        // Box2D:
        // 1) for each proxy compute the aabb with xf1 and xf2
        // 2) combine the aabbs
        // 3) calculate the displacement of the transforms
        // 4) have the broad phase move the proxy by the displacement
    }

    void SetFilter (const collision_filter& f)
    {
        this->filter = f;
        m_flags |= FILTER_DIRTY;
    }

    void Refilter (void)
    {
        throw "not yet implemented";
        // Box2D:
        // 1) for each contact list in the body
        // 2) if the contact fixture is this, set the contact's filter flag
        // 3) have the broad phase "touch" each proxy
        //
        // NOTE: 1 & 2 are not needed because of adding the DIRTY_FILTER flag.
        //       Still need to implement a better solution for point 3.
    }

    void FlagFilterClean (void) noexcept
    {
        m_flags &= ~FILTER_DIRTY;
    }

    void SetSensor (bool value) noexcept;

    bool IsSensor (void) const noexcept { return m_flags & SENSOR; }
    bool IsFilterDirty (void) const noexcept { return m_flags & FILTER_DIRTY; }

    mass_data ComputeMass (void) const noexcept
    {
        return this->shape->compute_mass(this->density);
    }

    const aabb& GetAABB (void) const noexcept
    {
        return proxy->box;
    }

    RigidBody*     body = nullptr;
    void*          user_data = nullptr;
    ishape*        shape = nullptr;
    fixture_proxy* proxy = nullptr;

    float density = 0.f;
    float friction = 0.2f;
    float restitution = 0.f;

    collision_filter filter;

private:

    enum StateFlags
    {
        SENSOR       = 0x0001,
        FILTER_DIRTY = 0x0002
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
