//! \headerfile <rdge/physics/collision.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/isometry.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/vec2.hpp>

#include <SDL_assert.h>

// sites of import:
//
// http://www.iforce2d.net/b2dtut/collision-anatomy
// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

static constexpr float LINEAR_SLOP = 0.005f; //!< Collision tolerance

//! \struct collision_manifold
//! \brief Container for collision resolution details
struct collision_manifold
{
    uint32 count = 0;               //!< Number of collision points
    float depths[2] = { 0.f, 0.f }; //!< Penetration depths
    math::vec2 contacts[2];         //!< Contact points
    math::vec2 normal;              //!< Vector of resolution, or collision normal
};

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

} // namespace physics
} // namespace rdge
