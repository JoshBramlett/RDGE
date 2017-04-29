//! \headerfile <rdge/physics/collision.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

// sites of import:
//
// http://www.iforce2d.net/b2dtut/collision-anatomy
// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct collision_manifold
//! \brief Container for collision resolution details
struct collision_manifold
{
    uint32 count = 0;               //!< Number of collision points
    float depths[2] = { 0.f, 0.f }; //!< Penetration depths
    math::vec2 contacts[2];         //!< Contact points
    math::vec2 normal;              //!< Vector of resolution, or collision normal
};

struct collision_filter
{
    uint32 group    = 0;
    uint16 category = 0x0001;
    uint16 mask     = 0xFFFF;
};

struct fixture_profile
{
    //Shape shape;                //!< Fixture underlying shape
    void* user_data = nullptr;  //!< Custom opaque pointer

    float density = 0.f;        //!< Shape density in kg/m^2
    float friction = 0.2f;      //!< Normalized friction coefficient
    float restitution = 0.f;    //!< Normalized elasticity

    collision_filter filter;    //!< Collision filter
    bool is_sensor = false;     //!< If fixture generates a collision response
};

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

    fixture_profile profile;  // should I use this, or the data below?

    //Shape shape;
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

enum class BodyType : rdge::uint8
{
    STATIC = 0,
    KINEMATIC,
    DYNAMIC
};

class RigidBody
{
public:
    /*
    b2Fixture* CreateFixture(const b2FixtureDef* def);
    b2Fixture* CreateFixture(const b2Shape* shape, float32 density);
    void DestroyFixture(b2Fixture* fixture);

    void SetTransform(const b2Vec2& position, float32 angle);
    const b2Transform& GetTransform() const;
    const b2Vec2& GetPosition() const;
    float32 GetAngle() const;

    const b2Vec2& GetWorldCenter() const;
    const b2Vec2& GetLocalCenter() const;

    void SetLinearVelocity(const b2Vec2& v);
    const b2Vec2& GetLinearVelocity() const;
    void SetAngularVelocity(float32 omega);
    float32 GetAngularVelocity() const;

    void ApplyForce(const b2Vec2& force, const b2Vec2& point, bool wake);
    void ApplyForceToCenter(const b2Vec2& force, bool wake);
    void ApplyTorque(float32 torque, bool wake);
    void ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, bool wake);
    void ApplyLinearImpulseToCenter(const b2Vec2& impulse, bool wake);
    void ApplyAngularImpulse(float32 impulse, bool wake);

    float32 GetMass() const;
    float32 GetInertia() const;
    void GetMassData(b2MassData* data) const;
    void SetMassData(const b2MassData* data);
    void ResetMassData();

    b2Vec2 GetWorldPoint(const b2Vec2& localPoint) const;
    b2Vec2 GetWorldVector(const b2Vec2& localVector) const;
    b2Vec2 GetLocalPoint(const b2Vec2& worldPoint) const;
    b2Vec2 GetLocalVector(const b2Vec2& worldVector) const;

    b2Vec2 GetLinearVelocityFromWorldPoint(const b2Vec2& worldPoint) const;
    b2Vec2 GetLinearVelocityFromLocalPoint(const b2Vec2& localPoint) const;
    float32 GetLinearDamping() const;
    void SetLinearDamping(float32 linearDamping);
    float32 GetAngularDamping() const;
    void SetAngularDamping(float32 angularDamping);

    float32 GetGravityScale() const;
    void SetGravityScale(float32 scale);

    void SetType(b2BodyType type);
    b2BodyType GetType() const;

    void SetBullet(bool flag);
    bool IsBullet() const;

    void SetSleepingAllowed(bool flag);
    bool IsSleepingAllowed() const;
    void SetAwake(bool flag);
    bool IsAwake() const;
    void SetActive(bool flag);
    bool IsActive() const;

    void SetFixedRotation(bool flag);
    bool IsFixedRotation() const;

    b2Fixture* GetFixtureList();
    const b2Fixture* GetFixtureList() const;

    b2JointEdge* GetJointList();
    const b2JointEdge* GetJointList() const;

    b2ContactEdge* GetContactList();
    const b2ContactEdge* GetContactList() const;

    b2Body* GetNext();
    const b2Body* GetNext() const;

    void* GetUserData() const;
    void SetUserData(void* data);

    b2World* GetWorld();
    const b2World* GetWorld() const;
*/


    BodyType type = BodyType::STATIC;

    rdge::math::vec2 position;
    rdge::math::vec2 linear_velocity;

    float angle = 0.f;
    float angular_velocity = 0.f;
    float linear_damping = 0.f;
    float angular_damping = 0.f;
    float gravity_scale = 1.f;

    bool fixed_rotation = false;

    bool allow_sleep = true;
    bool awake = true;
    bool active = true;
    bool bullet = false;

    void* user_data = nullptr;


};

class CollisionGraph
{
public:
};

} // namespace rdge
