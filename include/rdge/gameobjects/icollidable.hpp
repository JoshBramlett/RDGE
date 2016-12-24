//! \headerfile <rdge/gameobjects/icollidable.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/15/2015

#pragma once

#include <rdge/graphics/rect.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gameobjects {

//! \class ICollidable
//! \brief Base interface for all game objects supporting collision detection
class ICollidable
{
public:
    //! \brief ICollidable dtor
    virtual ~ICollidable (void) noexcept = default;

    //! \brief Get the AABB collision box
    //! \details The AABB (Axis aligned bounding box) that has parallel edges
    //!          to both the x and y axis
    //! \returns AABB Rect object
    virtual rdge::rect CollisionAABB (void) const = 0;
};

} // namespace gameobjects
} // namespace rdge
