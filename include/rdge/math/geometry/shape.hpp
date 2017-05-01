//! \headerfile <rdge/math/geometry/shape.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/01/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace math {

enum class ShapeType : uint8
{
    CIRCLE = 1,
    POLYGON
};


    // TODO shell of a class.
struct shape
{


    //https://github.com/erincatto/Box2D/blob/master/Box2D/Box2D/Collision/Shapes/b2Shape.h

    virtual ~shape (void) = default;

    virtual ShapeType type (void) const noexcept = 0;
};

} // namespace math
} // namespace rdge
