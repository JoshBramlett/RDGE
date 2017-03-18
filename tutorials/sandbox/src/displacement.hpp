#pragma once

#include <rdge/core.hpp>
#include <rdge/math.hpp>
#include <rdge/gameobjects.hpp>

#include <vector>

struct displacement
{
    virtual ~displacement (void) noexcept = default;

    virtual void calculate (rdge::math::vec2& unit_vector,
                            const rdge::delta_time& dt,
                            int index = 0) = 0;
};

// TODO Move acceleration_displacement and velocity_displacement to the engine
struct acceleration_displacement : displacement
{
    // TODO Friction is not accounted for, so the player will go on forever
    //      HMH talks of ordinary differential equations
    void calculate (rdge::math::vec2& unit_vector,
                    const rdge::delta_time& dt,
                    int index = 0) override
    {
        rdge::math::vec2 acceleration = unit_vector;
        acceleration *= unit_length * base_velocities[index];

        // emulates friction - but acceleration constant would need to be jacked up
        //acceleration += velocity * -5.5f;

        unit_vector = ((.5f * acceleration) * (dt.seconds * dt.seconds)) +
                      (velocity * dt.seconds);
        velocity = (acceleration * dt.seconds) + velocity;
    }

    rdge::math::vec2 velocity;
    float unit_length = 0.f;
    std::vector<float> base_velocities;
};

struct velocity_displacement : displacement
{
    void calculate (rdge::math::vec2& unit_vector,
                    const rdge::delta_time& dt,
                    int index = 0) override
    {
        unit_vector *= unit_length * dt.seconds * base_velocities[index];
    }

    float unit_length = 0.f;
    std::vector<float> base_velocities;
};
