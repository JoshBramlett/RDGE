//! \headerfile <rdge/physics/solver.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/24/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/stack_array.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

class CollisionGraph;
class RigidBody;
class Contact;

//! \class Solver //! \brief Performs impulse resolution for contacting bodies
//! \details Performed every simulation step, contacting bodies are added and
//!          impulses are generated and applied to separate them.
class Solver
{
public:
    //!@{ Dynamcis properties
    //! \var Threshold for elastic collisions.  Any collision with a relative
    //!      linear velocity below the threshold is treated as inelastic.
    static constexpr float VELOCITY_THRESHOLD = 1.f;

    //! \var Controls how fast overlap is resolved.  A value of 1 would remove
    //!      overlap in a single step, but often leads to overshoot.
    static constexpr float SCALE_FACTOR = 0.2f;

    //! \var Maximum linear position correction during a single step.  Helps
    //!      to prevent overshoot.
    static constexpr float MAX_LINEAR_CORRECTION = 0.2f;

    //! \var Maximum linear velocity of a body.
    static constexpr float MAX_TRANSLATION = 2.f;

    //! \var Maximum angular velocity of a body.
    static constexpr float MAX_ROTATION = 0.5f * math::PI;
    //!@}

    //!@{ Sleep properties
    //! \var Body must have a linear velocity below threshold to sleep
    static constexpr float LINEAR_SLEEP_TOLERANCE = 0.01f;

    //! \var Body must have a angular velocity below threshold to sleep
    static constexpr float ANGULAR_SLEEP_TOLERANCE = 2.0f / 180.0f * math::PI;

    //! \var Time a body must be motionless to fall asleep.
    static constexpr float SLEEP_THRESHOLD = 0.5f;
    //!@}

    //! \brief Solver default ctor
    Solver (void) = default;

    //! \brief Initialize solver for the current time step
    //! \param [in] delta_time Time in seconds
    //! \param [in] body_count Maximum number of bodies
    //! \param [in] body_count Maximum number of contacts
    void Initialize (float delta_time, size_t body_count, size_t contact_count);

    //! \brief Add body to the solver
    //! \param [in] b \ref RigidBody to add
    void Add (RigidBody* b);

    //! \brief Add contact to the solver
    //! \param [in] b \ref Contact to add
    void Add (Contact* c);

    //! \brief Clear all bodies and contacts from the solver
    void Clear (void);

    //! \brief Perform impulse resolution
    //! \details Velocity constraints are solved and positions are corrected.
    //!          Bodies positions and velocities are updated with the impulses
    //!          generated by the solver.
    void Solve (void);

    //! \brief Update island bodies and contacts
    //! \details Responsible for firing the \ref OnPostSolve event for each
    //!          contact, and sets bodies to sleep if applicable.
    void ProcessPostSolve (const CollisionGraph& graph);

private:

    void SolveVelocityConstraints (void);
    bool CorrectPositions (void);

public:

    //!@{ Global configuration
    math::vec2 gravity = { 0.f, -9.8f }; //!< Gravitational force
    size_t velocity_iterations = 8;      //!< Number of velocity constraint iterations
    size_t position_iterations = 3;      //!< Number of position correction iterations
    //!@}

private:

    //! \struct solver_body_data
    //! \brief Cache-friendly body relevant data
    struct solver_body_data
    {
        RigidBody* body;
        math::vec2 pos;         //!< maps to sweep.pos_n
        math::vec2 linear_vel;  //!< maps to linear.velocity
        float      angle;       //!< maps to sweep.angle_n
        float      angular_vel; //!< maps to angular.velocity
        float      inv_mass;    //!< maps to linear.inv_mass
        float      inv_mmoi;    //!< maps to angular.inv_mmoi
    };

    //! \struct solver_contact_data
    //! \brief Cache-friendly contact relevant data
    struct solver_contact_data
    {
        Contact* contact;
        size_t body_index[2];    //!< Indices of bodies in the \ref m_bodies container
        float combined_inv_mass; //!< Combined inverse mass of contacting bodies

        struct velocity_constraint_point
        {
            math::vec2 rel_point[2];     //!< (manifold.point - body.pos)
            float normal_impulse = 0.f;
            float tangent_impulse = 0.f;
            float normal_mass;
            float tangent_mass;
            float velocity_bias;
        };

        velocity_constraint_point points[2];
    };

    //!@{ Private members which are used and reset every time step
    stack_array<solver_body_data>    m_bodies;
    stack_array<solver_contact_data> m_contacts;
    float m_dt;
    bool m_positionsSolved;
    //!@}
};

} // namespace physics
} // namespace rdge
