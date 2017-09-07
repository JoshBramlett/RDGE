//! \headerfile <rdge/physics/base_joint.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/27/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SmallBlockAllocator;
//!@}

namespace physics {

//!@{ Forward declarations
class BaseJoint;
class CollisionGraph;
class Solver;
class RigidBody;
struct time_step;
struct solver_body_data;
//!@}

//! \enum JointType
//! \brief Types of supported joints
enum class JointType : uint8
{
    NONE = 0, //!< No type
    REVOLUTE  //!< \ref RevoluteJoint
};

//! \struct joint_edge
//! \brief Represents a joint between two bodies
//! \details The bodies represent nodes in a graph and the joint is the edge
//!          between them.  This is used when determining which bodies make
//!          up an island.
struct joint_edge : public intrusive_list_element<joint_edge>
{
    RigidBody* other = nullptr; //!< Body connected by the edge
    BaseJoint* joint = nullptr; //!< Joint connecting the bodies
};

//! \class BaseJoint
//! \brief Represents constrained motion between two bodies
class BaseJoint : public intrusive_list_element<BaseJoint>
{
public:
    //!@{ Non-copyable, Non-movable
    BaseJoint (const BaseJoint&) = delete;
    BaseJoint (BaseJoint&&) = delete;
    BaseJoint& operator= (const BaseJoint&) = delete;
    BaseJoint& operator= (BaseJoint&&) = delete;
    //!@}

    //! \returns Type of the joint
    virtual JointType Type (void) const noexcept = 0;

    //!@{ Anchors in world coordinates
    virtual math::vec2 AnchorA (void) const noexcept = 0;
    virtual math::vec2 AnchorB (void) const noexcept = 0;
    //!@}

    //! \returns If bodies should collide with one another
    bool ShouldCollide (void) const noexcept { return m_flags & BODIES_COLLIDABLE; }
    //bool AllowCollisions (bool allow) noexcept;

    //!@{ \ref RigidBody nodes linked by this joint
    RigidBody* body_a = nullptr;
    RigidBody* body_b = nullptr;
    //!@}

    //!@{ Pointers to edges stored by each \ref RigidBody
    joint_edge edge_a;
    joint_edge edge_b;
    //!@}

protected:

    friend class CollisionGraph;
    friend class Solver;
    friend class rdge::SmallBlockAllocator;

    BaseJoint (void) = default;
    virtual ~BaseJoint (void) noexcept = default;

    enum class LimitState
    {
        INACTIVE,
        AT_LOWER,
        AT_UPPER,
        EQUAL
    };

    virtual void InitializeSolver (const time_step&  step,
                                   solver_body_data& bdata_a,
                                   solver_body_data& bdata_b) = 0;
    virtual void SolveVelocityConstraints (const time_step&  step,
                                           solver_body_data& bdata_a,
                                           solver_body_data& bdata_b) = 0;
    virtual bool SolvePositionConstraints (solver_body_data& bdata_a,
                                           solver_body_data& bdata_b) = 0;

    enum BaseStateFlags
    {
        BODIES_COLLIDABLE = 0x0001,
        ON_ISLAND         = 0x0002
    };

    uint16 m_flags = 0;
};

} // namespace physics
} // namespace rdge
