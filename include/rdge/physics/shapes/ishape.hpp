//! \headerfile <rdge/physics/shapes/ishape.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/isometry.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

enum class ShapeType : uint8
{
    CIRCLE = 1,
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

// TODO shell of a class.
struct ishape
{


    //https://github.com/erincatto/Box2D/blob/master/Box2D/Box2D/Collision/Shapes/b2Shape.h
    // TODO raycast

    virtual ~ishape (void) = default;

    virtual ShapeType type (void) const = 0;
    virtual bool contains (const iso_transform& xf, const math::vec2& p) const = 0;
    virtual aabb compute_aabb (const iso_transform& xf) const = 0;
    virtual mass_data compute_mass (float density) const = 0;
};

inline std::ostream& operator<< (std::ostream& os, const mass_data& data)
{
    return os << "[ centroid=" << data.centroid
              << " mass=" << data.mass
              << " mmoi=" << data.mmoi << " ]";

}

} // namespace physics
} // namespace rdge
