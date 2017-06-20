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

#include <ostream>

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
    bool flip_dominant = false;     //!< Manifold data is relative to shape b
};

struct contact_impulse
{
    float normalImpulses[2]; // b2_maxManifoldPoints
    float tangentImpulses[2];
    size_t count;
};

//! \struct half_plane
//! \brief 2d hyperplane (aka line)
//! \details Line that divides space into two infinite sets of points.  Points on
//!          the plane satisfy dot(normal, p) == d.
//! \note From Real-Time Collision Detection, Vol 1. (3.6 Planes and Halfspaces)
struct half_plane
{
    math::vec2 normal; //!< Plane normal (normalized)
    float d;           //!< distance to origin from plane
};

//! \brief Distance from a point to the plane
//! \param [in] hp Half-plane
//! \param [in] point Point to test
//! \returns Distance
constexpr float
distance (const half_plane& hp, const math::vec2& point)
{
    return math::dot(hp.normal, point) - hp.d;
}

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
    const ishape* shape_a = nullptr;
    const ishape* shape_b = nullptr;

    math::vec2 simplex[3]; //!< Triangle surrounding the origin
    size_t count = 0;      //!< Number of vertices in the simplex
    math::vec2 d;          //!< Current search direction

    //! \brief gjk ctor
    //! \details Initializes the first point in the simplex and corresponding
    //!          direction for the intersection test to start.
    //! \param [in] a First shape
    //! \param [in] b Second shape
    gjk (const ishape* a, const ishape* b)
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
        // TODO This causes a segfault if called twice.  Could be converted to a function
        //      instead of a class.
        SDL_assert(count == 1);

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

            auto ac_perp = ac.perp_ccw() * math::perp_dot(ac, ab);
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
                auto ab_perp = ab.perp_ccw() * math::perp_dot(ab, ac);
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

//! \brief collision_manifold stream output operator
inline std::ostream& operator<< (std::ostream& os, const collision_manifold& mf)
{
    if (mf.count == 0)
    {
        return os << "[ manifold: count=0 ]\n";
    }

    os << "manifold: ["
       << "\n  count=" << mf.count
       << "\n  normal=" << mf.normal
       << "\n  flip_dominant=" << std::boolalpha << mf.flip_dominant;

    for (size_t i = 0; i < mf.count; i++)
    {
        os << "\n  contacts[" << i << "]=" << mf.contacts[i]
           << " depths[" << i << "]=" << mf.depths[i];
    }

    return os << "\n]\n";
}

} // namespace physics
} // namespace rdge
