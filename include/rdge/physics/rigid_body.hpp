//! \headerfile <rdge/physics/rigid_body.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/collision.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

//! \enum RigidBodyType
//! \brief Defines how a body acts during simulation
enum class RigidBodyType : uint8
{
    STATIC = 0, //!< No mass or velocity, only collides with dynamic bodies
    KINEMATIC,  //!< Does not react to forces, only collides with dynamic bodies
    DYNAMIC     //!< Fully simulated
};

//! \struct rigid_body_profile
//! \brief Profile for constructing a \ref RigidBody
struct rigid_body_profile
{
    void* user_data = nullptr;    //!< Opaque pointer

    math::vec2 position;          //!< World position
    math::vec2 linear_velocity;   //!< Linear velocity of the body's origin

    float angle = 0.f;            //!< Angle in radians
    float angular_velocity = 0.f; //!< Angular velocity

    float linear_damping = 0.f;   //!< Coefficient to reduce linear velocity
    float angular_damping = 0.f;  //!< Coefficient to reduce angular velocity

    float gravity_scale = 1.f;    //!< Normalized scale of the gravitational impact

    bool active = true;           //!< Body is initially active
    bool awake = true;            //!< Body is initially awake
    bool bullet = false;          //!< High velocity body - prevents tunneling
    bool fixed_rotation = false;  //!< Prevent rotation
    bool allow_sleep = true;      //!< Body is allowed to enter a sleep state

    RigidBodyType type = RigidBodyType::STATIC; //!< Canonical type defining the body
};

//! \class RigidBody
//! \brief Base physics simulation object
//! \details Maintains a position and velocity, and contains a collection of all
//!          objects that further define itself in the physical world.
class RigidBody
{
public:



    //b2Fixture* CreateFixture(const b2FixtureDef* def);
    //b2Fixture* CreateFixture(const b2Shape* shape, float32 density);
    //void DestroyFixture(b2Fixture* fixture);

    //void SetTransform(const b2Vec2& position, float32 angle);
    //const b2Transform& GetTransform() const;

    //void SetLinearVelocity(const b2Vec2& v);
    //const b2Vec2& GetLinearVelocity() const;
    //void SetAngularVelocity(float32 omega);
    //float32 GetAngularVelocity() const;

    //void ApplyForce(const b2Vec2& force, const b2Vec2& point, bool wake);
    //void ApplyForceToCenter(const b2Vec2& force, bool wake);
    //void ApplyTorque(float32 torque, bool wake);
    //void ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, bool wake);
    //void ApplyLinearImpulseToCenter(const b2Vec2& impulse, bool wake);
    //void ApplyAngularImpulse(float32 impulse, bool wake);

    //float32 GetMass() const;
    //float32 GetInertia() const;
    //void GetMassData(b2MassData* data) const;
    //void SetMassData(const b2MassData* data);
    //void ResetMassData();

    //b2Vec2 GetWorldPoint(const b2Vec2& localPoint) const;
    //b2Vec2 GetWorldVector(const b2Vec2& localVector) const;
    //b2Vec2 GetLocalPoint(const b2Vec2& worldPoint) const;
    //b2Vec2 GetLocalVector(const b2Vec2& worldVector) const;

    //b2Vec2 GetLinearVelocityFromWorldPoint(const b2Vec2& worldPoint) const;
    //b2Vec2 GetLinearVelocityFromLocalPoint(const b2Vec2& localPoint) const;

    //void SetType(b2BodyType type);

    //void SetBullet(bool flag);

    //void SetSleepingAllowed(bool flag);
    //void SetActive(bool flag);

    //void SetFixedRotation(bool flag);

    //b2Fixture* GetFixtureList();
    //const b2Fixture* GetFixtureList() const;

    //b2JointEdge* GetJointList();
    //const b2JointEdge* GetJointList() const;

    //b2ContactEdge* GetContactList();
    //const b2ContactEdge* GetContactList() const;

    //b2Body* GetNext();
    //const b2Body* GetNext() const;

    //b2World* GetWorld();
    //const b2World* GetWorld() const;

    RigidBodyType GetType (void) const noexcept { return m_type; }
    const math::vec2& GetPosition (void) const noexcept { return m_transform.pos; }
    float GetAngle (void) const noexcept { return m_sweep.angle_n; }
    const math::vec2& GetWorldCenter (void) const noexcept { return m_sweep.pos_n; }
    const math::vec2& GetLocalCenter (void) const noexcept { return m_sweep.local_center; }

    bool IsActive (void) const noexcept { return m_flags & ACTIVE_FLAG; }
    bool IsAwake (void) const noexcept { return m_flags & AWAKE_FLAG; }
    bool IsBullet (void) const noexcept { return m_flags & BULLET_FLAG; }
    bool IsFixedRotation (void) const noexcept { return m_flags & FIXED_ROTATION_FLAG; }
    bool IsSleepingAllowed (void) const noexcept { return m_flags & AUTOSLEEP_FLAG; }

    void Wake (void) noexcept
    {
        if ((m_flags & AWAKE_FLAG) == 0)
        {
            m_flags |= AWAKE_FLAG;
            m_sleepTime = 0.f;
        }
    }

    void Sleep (void) noexcept
    {
        m_flags &= ~AWAKE_FLAG;
        m_sleepTime = 0.f;
        m_force = { 0.f, 0.f };
        m_torque = 0.f;
        this->linear_velocity = { 0.f, 0.f };
        this->angular_velocity = 0.f;
    }

public:

    void*      user_data = nullptr;

    math::vec2 linear_velocity;
    float      angular_velocity = 0.f;

    float      linear_damping = 0.f;
    float      angular_damping = 0.f;
    float      gravity_scale = 0.f;

private:

    enum InternalFlags
    {
        ACTIVE_FLAG         = 0x0091,
        AWAKE_FLAG          = 0x0002,
        BULLET_FLAG         = 0x0004,
        FIXED_ROTATION_FLAG = 0x0008,
        AUTOSLEEP_FLAG      = 0x0010,

        ISLAND_FLAG         = 0x0020,
        TOI_FLAG            = 0x0040
    };

    iso_transform m_transform;
    sweep_step    m_sweep;

    math::vec2 m_force;
    float      m_torque = 0.f;

    //float      m_mass = 0.f;
    //float      m_invMass = 0.f;
    //float      m_inertia = 0.f;
    //float      m_invInertia = 0.f;

    float      m_sleepTime = 0.f;

    uint16        m_flags = 0;
    RigidBodyType m_type;
};

} // namespace physics
} // namespace rdge
