//! \headerfile <rdge/gameobjects/icollidable.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/15/2015
//! \bug

#pragma once

#include <rdge/graphics/rect.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \class ICollidable
//! \brief Base interface for all game objects supporting collision detection
class ICollidable
{
public:
    //! \brief ICollidable dtor
    virtual ~ICollidable (void) = 0;

    //! \brief Get the AABB collision box
    //! \details The AABB (Axis aligned bounding box) that has parallel edges
    //!          to both the x and y axis
    //! \returns AABB Rect object
    virtual RDGE::Graphics::Rect CollisionAABB (void) const = 0;
};

} // namespace GameObjects
} // namespace RDGE
