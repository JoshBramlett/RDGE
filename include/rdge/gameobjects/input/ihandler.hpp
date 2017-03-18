//! \headerfile <rdge/gameobjects/input/ihandler.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/17/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/types.hpp>
#include <rdge/math/vec2.hpp>

#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class IDirectionalInputHandler
//! \brief Interface for directional input handling
class IDirectionalInputHandler
{
public:
    //! \brief IDirectionalInputHandler dtor
    virtual ~IDirectionalInputHandler (void) noexcept = default;

    //! \brief Event handler
    virtual void OnEvent (const Event&) = 0;

    //! \brief Calculate the relative displacement for a frame
    //! \details Coordinates represent a directional unit vector and should fall
    //!          on a unit circle.  Therefore calculations will still need to be
    //!          done to find the final position offset, such as multiplying the
    //!          delta time, velocity, etc.  Because the direction can be any angle
    //!          the facing direction is also provided, which is the major NESW
    //!          direction closest to the vector angle.
    //! \returns Pair containing the relative displacement and a NESW direction
    virtual std::pair<math::vec2, Direction> Calculate (void) = 0;
};

} // namespace rdge
