//! \headerfile <rdge/physics/joints/revolute_joint.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/26/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/joints/base_joint.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat3.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SmallBlockAllocator;
//!@}

namespace physics {

//!@{ Forward declarations
class CollisionGraph;
class Solver;
struct time_step;
struct solver_body_data;
//!@}

//! \class RevoluteJoint
//! \brief Constrains two bodies rotation around a common point
//! \details Bodies are allowed to rotate freely (but not translate) about a common
//!          point.  The system can translate and rotate freely.
class RevoluteJoint : public BaseJoint
{
public:
    //!@{ Non-copyable, Non-movable
    RevoluteJoint (const RevoluteJoint&) = delete;
    RevoluteJoint (RevoluteJoint&&) = delete;
    RevoluteJoint& operator= (const RevoluteJoint&) = delete;
    RevoluteJoint& operator= (RevoluteJoint&&) = delete;
    //!@}

    //! \returns Type of the joint
    JointType Type (void) const noexcept override { return JointType::REVOLUTE; }

    //!@{ Anchors in world coordinates
    math::vec2 AnchorA (void) const noexcept override;
    math::vec2 AnchorB (void) const noexcept override;
    //!@}

    //! \returns Relative angle between the two rotating bodies
    float JointAngle (void) const noexcept;

    //! \returns Relative speed at which the bodies are rotating (radians/sec)
    float JointSpeed (void) const noexcept;

    bool IsMotorEnabled (void) const noexcept { return m_flags & MOTOR_ENABLED; }
    void EnableMotor (void) noexcept;
    void DisableMotor (void) noexcept;
    void SetMotorSpeed (float speed) noexcept;
    void SetMaxMotorTorque (float torque) noexcept;

    bool IsLimitsEnabled (void) const noexcept { return m_flags & LIMIT_ENABLED; }
    void EnableLimits (void) noexcept;
    void DisableLimits (void) noexcept;
    void SetLimits (float lower, float upper) noexcept;

private:

    friend class CollisionGraph;
    friend class Solver;
    friend class rdge::SmallBlockAllocator;
    friend std::ostream& operator<< (std::ostream&, const RevoluteJoint&);

    explicit RevoluteJoint (RigidBody* a, RigidBody* b, const math::vec2& anchor);
    ~RevoluteJoint (void) noexcept = default;

    void InitializeSolver (const time_step&  step,
                           solver_body_data& bdata_a,
                           solver_body_data& bdata_b) override;
    void SolveVelocityConstraints (const time_step&  step,
                                   solver_body_data& bdata_a,
                                   solver_body_data& bdata_b) override;
    bool SolvePositionConstraints (solver_body_data& bdata_a,
                                   solver_body_data& bdata_b) override;

    math::vec2 m_anchor[2];          //!< Anchors local to the respective bodies
    math::mat3 m_mass;               //!< Effective mass for point-to-point constraint
	float      m_referenceAngle = 0.f;
    float      m_motorMass = 0.f;    //!< Effective mass for motor/limit angular constraint
    math::vec3 m_impulse;            //!< Includes reaction force (x,y), and torque (z)
    float      m_motorImpulse = 0.f;

    //!@{ Motor properties
    float m_maxMotorTorque = 0.f;
    float m_motorSpeed = 0.f;
    //!@}

    //!@{ Angle limit properties
    LimitState m_limitState = LimitState::INACTIVE;
	float m_lowerAngle = 0.f;
	float m_upperAngle = 0.f;
    //!@}

    //!@{ Cached for solver
    math::vec2 m_localCenterA;
    math::vec2 m_localCenterB;
    //!@}

    // IMPORTANT First 8 bytes reserved by the base class
    enum StateFlags
    {
        LIMIT_ENABLED = 0x0010,
        MOTOR_ENABLED = 0x0020
    };
};

//! \brief RevoluteJoint stream output operator
std::ostream& operator<< (std::ostream& os, const RevoluteJoint& value);

} // namespace physics
} // namespace rdge
