//! \headerfile <rdge/physics/rigid_body.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/util/containers/nodeless_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class RigidBody;
class CollisionGraph;
class Contact;

// Box2D dependency tree:
// Body
//   Fixture
//   World
//   ContactManager
//   ContactEdge
//   Contact
//   BroadPhase
//   BlockAllocator

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

//! \class RigidBody
//! \brief Base physics simulation object
//! \details Maintains a position and velocity, and contains a collection of all
//!          objects that further define itself in the physical world.
class RigidBody : public nodeless_list_element<RigidBody>
{
public:

    explicit RigidBody (const rigid_body_profile& profile,
                        CollisionGraph*           parent);
    ~RigidBody (void) noexcept;

    Fixture* CreateFixture (const fixture_profile& profile);
    void DestroyFixture (Fixture* fixture);
    void SynchronizeFixtures (void);
    void ComputeMass (void);

    //void SetTransform(const b2Vec2& position, float32 angle);
    //void SetLinearVelocity(const b2Vec2& v);
    //void SetAngularVelocity(float32 omega);

    //void ApplyForce(const b2Vec2& force, const b2Vec2& point, bool wake);
    //void ApplyForceToCenter(const b2Vec2& force, bool wake);
    //void ApplyTorque(float32 torque, bool wake);
    //void ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, bool wake);
    //void ApplyLinearImpulseToCenter(const b2Vec2& impulse, bool wake);
    //void ApplyAngularImpulse(float32 impulse, bool wake);

    //b2Vec2 GetWorldPoint(const b2Vec2& localPoint) const;
    //b2Vec2 GetWorldVector(const b2Vec2& localVector) const;
    //b2Vec2 GetLocalPoint(const b2Vec2& worldPoint) const;
    //b2Vec2 GetLocalVector(const b2Vec2& worldVector) const;

    //b2Vec2 GetLinearVelocityFromWorldPoint(const b2Vec2& worldPoint) const;
    //b2Vec2 GetLinearVelocityFromLocalPoint(const b2Vec2& localPoint) const;

    //void SetBullet(bool flag);

    //void SetSleepingAllowed(bool flag);
    //void SetActive(bool flag);

    //void SetFixedRotation(bool flag);

    RigidBodyType GetType (void) const noexcept { return m_type; }
    // world position of the body origin
    const math::vec2& GetPosition (void) const noexcept { return world_transform.pos; }
    float GetAngle (void) const noexcept { return m_sweep.angle_n; }
    // world position of the body center of mass
    const math::vec2& GetWorldCenter (void) const noexcept { return m_sweep.pos_n; }
    const math::vec2& GetLocalCenter (void) const noexcept { return m_sweep.local_center; }


    bool IsAwake (void) const noexcept
    {
        // Static bodies are always sleeping
        // TODO This behavior differs from Box2D
        return (m_type == RigidBodyType::STATIC) || (m_flags & AWAKE_FLAG);
    }

    bool IsActive (void) const noexcept { return m_flags & ACTIVE_FLAG; }
    bool IsBullet (void) const noexcept { return m_flags & BULLET_FLAG; }
    bool IsFixedRotation (void) const noexcept { return m_flags & FIXED_ROTATION_FLAG; }
    bool IsSleepingAllowed (void) const noexcept { return m_flags & AUTOSLEEP_FLAG; }

    bool ShouldCollide (RigidBody* other)
    {
        return m_type == RigidBodyType::DYNAMIC ||
               other->m_type == RigidBodyType::DYNAMIC;
    }

    void WakeUp (void) noexcept
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
        this->linear.force = { 0.f, 0.f };
        this->angular.torque = 0.f;
        this->linear.velocity = { 0.f, 0.f };
        this->angular.velocity = 0.f;
    }

public:

    CollisionGraph* graph = nullptr;
    void* user_data = nullptr;

    nodeless_forward_list<Fixture> fixtures;
    nodeless_list<contact_edge> contacts;

    //! \brief Collection of elements defining the linear motion
    struct linear_motion
    {
        math::vec2 velocity;
        math::vec2 force;
        float      damping = 0.f;
        float      mass = 0.f;
        float      inv_mass = 0.f;
    } linear;

    //! \brief Collection of elements defining the angular motion
    struct angular_motion
    {
        float velocity = 0.f;
        float torque = 0.f;
        float damping = 0.f;
        float mmoi = 0.f;
        float inv_mmoi = 0.f;
    } angular;

    //! \brief Linear/angular transforms to represent the body in world space
    iso_transform world_transform;

    float gravity_scale = 0.f; //!< Gravitational impact on the body

private:

    enum InternalFlags
    {
        ACTIVE_FLAG         = 0x0001,
        AWAKE_FLAG          = 0x0002,
        BULLET_FLAG         = 0x0004,
        FIXED_ROTATION_FLAG = 0x0008,
        AUTOSLEEP_FLAG      = 0x0010,

        ISLAND_FLAG         = 0x0020,
        TOI_FLAG            = 0x0040
    };

    //! \brief Contains the local center of mass, and the position/angle over the
    //!        the timestep.
    sweep_step m_sweep;

    float      m_sleepTime = 0.f;

    uint16        m_flags = 0;
    RigidBodyType m_type;
};

} // namespace physics
} // namespace rdge
