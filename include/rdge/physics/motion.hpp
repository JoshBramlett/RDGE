//! \headerfile <rdge/physics/motion.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/13/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/iscene.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct displacement
//! \brief Calculates a displacement vector for a given timestep
struct displacement
{
    float unit        = 0.f; //!< Base unit (e.g. meters)
    float coefficient = 0.f; //!< Static scalar coefficient (e.g. units/second)

    //! \brief Generate a displacement vector from a velocity coefficient
    //! \param [in] unit_vector Direction (normalized to a unit circle)
    //! \param [in] dt Delta time
    //! \returns Displacement vector
    math::vec2 from_velocity (const math::vec2& unit_vector, const delta_time& dt)
    {
        return unit_vector * unit * coefficient * dt.seconds;
    }

    //! \brief Generate a displacement vector from an acceleration coefficient
    //! \param [in] unit_vector Direction (normalized to a unit circle)
    //! \param [in] dt Delta time
    //! \returns Displacement vector
    math::vec2 from_acceleration (const math::vec2& unit_vector, const delta_time& dt)
    {
        // TODO This is ripe with issues, but I'm leaving it here to revisit as it
        //      illustrates how an acceleration coefficient integrates to determine
        //      the displacement vector.
        //
        //      http://www.physicsclassroom.com/class/1DKin/Lesson-1/Acceleration
        //
        //      Problems were with friction, while controlling a player character.
        //      I was using friction to "stop" the player more abruptly when not
        //      moving or changing directions.

        float friction = 0.75f;
        if (unit_vector.x == 0.f || math::sign(unit_vector.x) != math::sign(m_velocity.x))
        {
            m_velocity.x *= friction;
        }

        if (unit_vector.y == 0.f || math::sign(unit_vector.y) != math::sign(m_velocity.y))
        {
            m_velocity.y *= friction;
        }

        math::vec2 acceleration = unit_vector * coefficient;
        math::vec2 result = ((.5f * acceleration) * (dt.seconds * dt.seconds)) +
                            (m_velocity * dt.seconds);
        m_velocity += (acceleration * dt.seconds);

        // clamp to terminal velocity
        m_velocity = math::clamp(m_velocity, -coefficient, coefficient);

        return result * unit;
    }

private:
    math::vec2 m_velocity; //!< Cached velocity (used internally)
};

} // namespace rdge
