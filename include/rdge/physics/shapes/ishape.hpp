//! \headerfile <rdge/physics/shapes/ishape.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/isometry.hpp>
#include <rdge/math/vec2.hpp>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

enum class ShapeType : uint8
{
    INVALID = 0,
    CIRCLE,
    POLYGON
};

//! \struct mass_data
//! \brief Container for a shape's computed mass data
struct mass_data
{
    math::vec2 centroid;   //!< Centroid relative to the local origin
    float      mass = 0.f; //!< Shape mass (area x density)

    //! \var mmoi
    //! \brief Mass moment of inertia (aka rotational inertia)
    //! \details Mass moment of inertia is the angular analog of mass, and is a tensor
    //!          which determines the amount of torque required for angular acceleration.
    float mmoi = 0.f;
};

//! \struct ishape
//! \brief Abstract interface for a convex shape
//! \details Shapes inheriting from the ishape iterface contain functionality
//!          supporting multiple aspects of the simulation, including narrow
//!          phase collision routines, creating broad phase proxy aabb wrappers,
//!          and computation of mass data.
struct ishape
{

    //https://github.com/erincatto/Box2D/blob/master/Box2D/Box2D/Collision/Shapes/b2Shape.h
    // TODO raycast

    virtual ~ishape (void) noexcept = default;

    //!@{ Shape properties
    virtual ShapeType type (void) const = 0;
    virtual math::vec2 get_centroid (void) const = 0;
    //!@}

    //! \brief Transform the shape to world space
    virtual void to_world (const iso_transform& xf) = 0;

    //! \brief Compute aabb wrapper for use in the broad phase
    virtual aabb compute_aabb (void) const = 0;
    virtual aabb compute_aabb (const iso_transform& xf) const = 0;

    //! \brief Compute mass data used in physics simulation
    virtual mass_data compute_mass (float density) const = 0;

    //!@{ Narrow phase collision detection routines
    virtual bool contains (const math::vec2& point) const = 0;
    virtual bool intersects_with (const ishape* other) const = 0;
    virtual bool intersects_with (const ishape* other, collision_manifold& mf) const = 0;
    //!@}

    //!@{ SAT support functions
    virtual math::vec2 project (const math::vec2& axis) const = 0;
    //!@}

    //!@{ GJK support functions
    virtual math::vec2 first_point (void) const = 0;
    virtual math::vec2 farthest_point (const math::vec2& d) const = 0;
    //!@}
};

//! \brief mass_data stream output operator
std::ostream& operator<< (std::ostream&, const mass_data&);

//! \brief ShapeType stream output operator
std::ostream& operator<< (std::ostream&, ShapeType);

} // namespace physics

//!@{ ShapeType string conversions
std::string to_string (physics::ShapeType);
bool try_parse (const std::string&, physics::ShapeType);
//!@}

} // namespace rdge
