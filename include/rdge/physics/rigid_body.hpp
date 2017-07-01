//! \headerfile <rdge/physics/rigid_body.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/29/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/contact.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

class SmallBlockAllocator;

namespace physics {

class RigidBody;
class CollisionGraph;
class Contact;

//! \struct sweep_step
//! \brief Describes the motion of a body/shape during the time step
//! \details Stores an advancing time and caches the position and angle at
//!          that time period (pos_0 and angle_0 are at the time alpha_0).
//! \see https://www.gamedev.net/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084
struct sweep_step
{
    math::vec2 local_center; //!< Local center of mass position
    math::vec2 pos_0;        //!< World position at alpha_0
    math::vec2 pos_n;        //!< World position at frame end
    float angle_0 = 0.f;     //!< World angle at alpha_0
    float angle_n = 0.f;     //!< World angle at frame end

    float alpha_0 = 0.f;     //!< Normalized fraction of the current time step

    //! \brief Calculate the interpolated transform for a given time
    //! \param [in] beta Normalized time fraction, where 0 indicates alpha_0
    //! \returns Interpolated transform
    iso_transform lerp_transform (float beta) noexcept
    {
        SDL_assert(0.f <= beta && beta <= 1.f);

        iso_transform result(((1.f - beta) * pos_0) + (beta * pos_n),
                             ((1.f - beta) * angle_0) + (beta * angle_n));

        result.pos -= result.rot.rotate(local_center);
        return result;
    }

    //! \brief Advance the sweep forward, yielding a new initial state
    //! \param [in] alpha The new \ref alpha_0
    void advance (float alpha) noexcept
    {
        SDL_assert(0.f <= alpha && alpha <= 1.f);

        float beta = (alpha - alpha_0) / (1.f - alpha_0);
        pos_0 += ((pos_n - pos_0) * beta);
        angle_0 += ((angle_n - angle_0) * beta);

        alpha_0 = alpha;
    }

    //! \brief Normalize the angle in radians between -pi and pi
    void normalize (void) noexcept
    {
        float d = math::TWO_PI * std::floorf(angle_0 / math::TWO_PI);
        angle_0 -= d;
        angle_n -= d;
    }
};

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
    RigidBodyType type = RigidBodyType::STATIC; //!< Canonical type defining the body

    float gravity_scale = 1.f;     //!< Normalized scale of the gravitational impact
    void* user_data = nullptr;     //!< Opaque user data

    //!@{ Linear properties
    math::vec2 position;           //!< World position
    math::vec2 linear_velocity;    //!< Linear velocity of the body's origin
    float linear_damping = 0.f;    //!< Coefficient to reduce linear velocity
    //!@}

    //!@{ Angular properties
    float angle = 0.f;             //!< Angle in radians
    float angular_velocity = 0.f;  //!< Angular velocity
    float angular_damping = 0.f;   //!< Coefficient to reduce angular velocity
    //!@}

    //!@{ RigidBody state flags
    bool simulate = true;          //!< Include body in physics simulation
    bool awake = true;             //!< Body is initially awake
    bool prevent_rotation = false; //!< Prevent rotation
    bool prevent_sleep = false;    //!< Keep body awake
    //!@}
};

//! \class RigidBody
//! \brief Base physics simulation object
//! \details Maintains a position and velocity, and contains a collection of all
//!          objects that further define itself in the physical world.
class RigidBody : public intrusive_list_element<RigidBody>
{
public:

    //! \brief Create a fixture and attach it to this body
    //! \details Initializes a fixture from the provided profile.  If the
    //!          body is simulating contacts will be added during the next
    //!          time step.  Mass data is automatically re-calculated.
    //! \param [in] profile Profile defining the fixture
    //! \returns Pointer to the created fixture
    //! \warning Function is locked during simulation
    Fixture* CreateFixture (const fixture_profile& profile);

    //! \brief Specialized overload for shape/density
    //! \param [in] shape Shape of the fixture
    //! \param [in] density Density of the fixture
    //! \returns Pointer to the created fixture
    //! \warning Function is locked during simulation
    Fixture* CreateFixture (ishape* shape, float density);

    //! \brief Destroy an attached fixture
    //! \details If the body is simulating contacts associated with the fixture
    //!          are destroyed.  Mass data is automatically re-calculated.
    //! \param [in] fixture Fixture to destroy
    //! \warning Function is locked during simulation
    void DestroyFixture (Fixture* fixture);

    math::vec2 GetLinearVelocityFromWorldPoint (const math::vec2& point)
    {
        return linear.velocity + ((point - sweep.pos_n).perp() * angular.velocity);
    }

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
    float GetAngle (void) const noexcept { return sweep.angle_n; }
    // world position of the body center of mass
    const math::vec2& GetWorldCenter (void) const noexcept { return sweep.pos_n; }
    const math::vec2& GetLocalCenter (void) const noexcept { return sweep.local_center; }



    //! \brief Check if body is participating in the physics simulation
    bool IsSimulating (void) const noexcept { return m_flags & SIMULATE; }

    //! \brief Check if body is awake
    //! \note Static bodies are always in a sleeping state
    bool IsAwake (void) const noexcept
    {
        // TODO PREVENT_SLEEP
        return (m_type != RigidBodyType::STATIC) && (m_flags & AWAKE);
    }

    bool IsSleepPrevented (void) const noexcept
    {
        return (m_type == RigidBodyType::STATIC) || (m_flags & PREVENT_SLEEP);
    }

    void WakeUp (void) noexcept
    {
        if (!IsAwake())
        {
            m_flags |= AWAKE;
            m_sleepTime = 0.f;
        }
    }

    void Sleep (void) noexcept
    {
        if (IsAwake())
        {
            m_flags &= ~AWAKE;
            m_sleepTime = 0.f;

            linear.force = { 0.f, 0.f };
            angular.torque = 0.f;
            linear.velocity = { 0.f, 0.f };
            angular.velocity = 0.f;
        }
    }


    bool IsFixedRotation (void) const noexcept { return m_flags & PREVENT_ROTATION; }

    bool ShouldCollide (RigidBody* other) const noexcept
    {
        return (this != other) &&
               (m_type == RigidBodyType::DYNAMIC || other->m_type == RigidBodyType::DYNAMIC);
    }

private:

    friend class CollisionGraph;
    friend class Solver;
    friend class rdge::SmallBlockAllocator;

    //! \brief RigidBody ctor
    //! \details Initialized from the provided profile.  Creation is done
    //!          through \ref CollisionGraph::CreateBody.
    //! \param [in] profile Profile defining the object
    //! \param [in] parent Parent object responsible for creation
    explicit RigidBody (const rigid_body_profile& profile, CollisionGraph* parent);

    //! \brief RigidBody dtor
    //! \details Responsible for cleaning up child fixtures.
    ~RigidBody (void) noexcept;

    bool HasEdge (const Fixture* a, const Fixture* b) noexcept;
    void SyncFixtures (void);
    void ComputeMass (void);

public:

    CollisionGraph* graph = nullptr; //!< Circular reference to parent
    void* user_data = nullptr;       //!< Opaque user data

    intrusive_forward_list<Fixture> fixtures;
    intrusive_list<contact_edge> contact_edges;

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

    //! \brief Contains the local center of mass, and the position/angle over the
    //!        the timestep.
    sweep_step sweep;

    float gravity_scale = 0.f; //!< Gravitational impact on the body

    size_t solver_index;

private:

    enum StateFlags
    {
        SIMULATE         = 0x0001,
        AWAKE            = 0x0002,
        PREVENT_ROTATION = 0x0004,
        PREVENT_SLEEP    = 0x0008,

        ON_ISLAND        = 0x0010
    };

    float      m_sleepTime = 0.f;

    uint16        m_flags = 0;
    RigidBodyType m_type;
};

} // namespace physics
} // namespace rdge
