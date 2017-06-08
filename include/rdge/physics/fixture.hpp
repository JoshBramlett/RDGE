//! \headerfile <rdge/physics/fixture.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/nodeless_list.hpp>

#include <SDL_assert.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

class SmallBlockAllocator;

namespace physics {

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
//! \brief Container used by the broad phase
//! \details The proxy represents the fixture's wrapped AABB in world coordinates,
//!          used by the broad phase for collision detection.
struct fixture_proxy
{
    static constexpr int32 INVALID_HANDLE = -1;

    Fixture* fixture = nullptr;    //!< Circular reference
    aabb box;                      //!< AABB wrapper for the fixture shape
    int32 handle = INVALID_HANDLE; //!< Handle provided by the broad phase
};

class Fixture : public nodeless_forward_list_element<Fixture>
{
public:
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
        return m_shape->compute_mass(density);
    }

    aabb ComputeAABB (void) const noexcept
    {
        return m_shape->compute_aabb();
    }

    // TODO Maybe instead of recalculating every call I could do something similar
    //      to the SyncProxies methdod, where this is updated only if the transform
    //      is moved, then the cached lookup would be very fast.
    ishape* GetWorldShape (void) noexcept;

    RigidBody*     body = nullptr;      //!< Circular reference to parent
    void*          user_data = nullptr; //!< Opaque user data
    fixture_proxy* proxy = nullptr;     //!< Broad phase proxy

    float density = 0.f;
    float friction = 0.2f;
    float restitution = 0.f;

    collision_filter filter;

private:
    friend class RigidBody;
    friend class rdge::SmallBlockAllocator;

    explicit Fixture (const fixture_profile& profile, RigidBody* parent);
    ~Fixture (void) noexcept;

    enum StateFlags
    {
        SENSOR       = 0x0001,
        FILTER_DIRTY = 0x0002
    };

    ishape* m_shape = nullptr;      //!< Fixture shape
    ishape* m_worldShape = nullptr; //!< Shape transformed to world coordinates

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
