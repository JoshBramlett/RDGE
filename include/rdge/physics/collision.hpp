//! \headerfile <rdge/physics/collision.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/isometry.hpp>
#include <rdge/physics/shapes/ishape.hpp>
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

//! \struct gjk
//! \brief Implementation of the GJK algorithm for collision detection
//! \details The algorithm operates on two convex shapes, and performs it's
//!          operations using the Minkowski difference of the shapes.  If
//!          the shapes intersect the Minkowski difference will include the
//!          origin, so the algorithm will attempt to create a surrounding
//!          triangle around the origin.  The intersection test hinges on
//!          whether this triangle can be formed.
//! \see http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
//! \see http://mollyrocket.com/849
struct gjk
{
    ishape* shape_a = nullptr;
    ishape* shape_b = nullptr;

    math::vec2 simplex[3]; //!< Triangle surrounding the origin
    size_t count = 0;      //!< Number of vertices in the simplex
    math::vec2 d;          //!< Current search direction

    //! \brief gjk ctor
    //! \details Initializes the first point in the simplex and corresponding
    //!          direction for the intersection test to start.
    //! \param [in] a First shape
    //! \param [in] b Second shape
    gjk (ishape* a, ishape* b)
        : shape_a(a)
        , shape_b(b)
    {
        math::vec2 s = shape_a->first_point() - shape_b->first_point();
        simplex[count++] = s;
        d = -s;
    }

    //! \brief Check if shapes intersect
    //! \returns True iff shapes intersect
    bool intersects (void)
    {
        while (true)
        {
            auto a = support(d);
            if (a.dot(d) < 0.f)
            {
                // simplex does not pass the origin
                return false;
            }

            simplex[count++] = a;
            if (do_simplex())
            {
                return true;
            }
        }
    }

    //! \brief GJK support function
    //! \details Retrieves the farthest vertex in opposite directions.  The
    //!          resultant vertex is an edge of the Minkowski difference.
    //! \param [in] d Direction to search
    //! \returns Edge vertex on the Minkowski difference
    math::vec2 support (const math::vec2& d)
    {
        return shape_a->farthest_point(d) - shape_b->farthest_point(-d);
    }

    //! \brief Attempt to create the simplex
    //! \details If the simplex cannot be verified the search direction and
    //!          simplex will be updated for the next iteration.
    //! \returns True iff simplex contains the origin
    bool do_simplex (void)
    {
        const auto& a = simplex[count - 1];
        auto ao = -a;

        if (count == 3)
        {
            const auto& b = simplex[1];
            const auto& c = simplex[0];
            auto ab = b - a;
            auto ac = c - a;

            auto ac_perp = ac.perp() * math::perp_dot(ab, ac);
            if (ac_perp.dot(ao) > 0)
            {
                // If the edge AC normal has the same direction as the origin
                // we remove point B and set the direction to the normal.
                simplex[1] = a;
                count--;

                d = ac_perp;
            }
            else
            {
                auto ab_perp = ab.perp() * math::perp_dot(ac, ab);
                if (ab_perp.dot(ao) > 0)
                {
                    // If the edge AB normal has the same direction as the origin
                    // we remove point C and set the direction to the normal.
                    simplex[0] = b;
                    simplex[1] = a;
                    count--;

                    d = ab_perp;
                }
                else
                {
                    // The origin lies inside both the AB and AC edge
                    return true;
                }
            }
        }
        else
        {
            const auto& b = simplex[0];
            auto ab = b - a;

            d = ab.perp() * math::perp_dot(ab, ao);
        }

        return false;
    }
};

} // namespace physics
} // namespace rdge
