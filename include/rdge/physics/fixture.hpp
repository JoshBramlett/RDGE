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

#include <SDL_assert.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \struct collision_filter
//! \brief Collision filtering data
struct collision_filter
{
    uint16 group    = 0;
    uint16 category = 0x0001; //!< Category the filter belongs to
    uint16 mask     = 0xFFFF; //!< Mask of other categories the object can collide with
};

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
//! \brief Broad phase proxy
struct fixture_proxy
{
    aabb box;
    // Fixture* fixture = nullptr;
    // int32 proxy_id;
};

class RigidBody;

class Fixture
{
public:
    /*
**  b2Shape::Type GetType() const;
**  b2Shape* GetShape();
**  const b2Shape* GetShape() const;

    // Awakens the body
    void SetSensor(bool sensor);
**  bool IsSensor() const;

    // SetFilterData calls Refilter, which updates contacts and proxies in the
    // broad phase - need to look at this further
    void SetFilterData(const b2Filter& filter);
**  const b2Filter& GetFilterData() const;
    void Refilter();

**  b2Body* GetBody();
**  const b2Body* GetBody() const;

**  b2Fixture* GetNext();
**  const b2Fixture* GetNext() const;

**  void* GetUserData() const;
**  void SetUserData(void* data);

    bool TestPoint(const b2Vec2& p) const;
    bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input, int32 childIndex) const;

    void GetMassData(b2MassData* massData) const;
**  void SetDensity(float32 density);
**  float32 GetDensity() const;
**  float32 GetFriction() const;
**  void SetFriction(float32 friction);
**  float32 GetRestitution() const;
**  void SetRestitution(float32 restitution);

    const b2AABB& GetAABB(int32 childIndex) const;

protected:
	void Create(b2BlockAllocator* allocator, b2Body* body, const b2FixtureDef* def);
	void Destroy(b2BlockAllocator* allocator);

	// These support body activation/deactivation.
	void CreateProxies(b2BroadPhase* broadPhase, const b2Transform& xf);
	void DestroyProxies(b2BroadPhase* broadPhase);

	void Synchronize(b2BroadPhase* broadPhase, const b2Transform& xf1, const b2Transform& xf2);
    */

    explicit Fixture (const fixture_profile& profile, RigidBody* parent);
    ~Fixture (void) noexcept;

    mass_data GetMassData (void) const noexcept
    {
        SDL_assert(this->shape != nullptr);
        return this->shape->compute_mass(this->density);
    }

    RigidBody* body = nullptr;
    Fixture* next = nullptr;

    ishape* shape = nullptr;
    void* user_data = nullptr;

    float density = 0.f;
    float friction = 0.2f;
    float restitution = 0.f;

    collision_filter filter;
    bool is_sensor = false;

    /*
	b2FixtureProxy* m_proxies; // <-- defined in b2Fixture.h
	int32 m_proxyCount;
	b2Fixture* m_next;
	b2Body* m_body;
    */
};

} // namespace physics
} // namespace rdge
