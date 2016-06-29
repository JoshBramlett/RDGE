//! \headerfile <rdge/gameobjects/ientity.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/events/event.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \class IEntity
//! \brief Base interface for all game objects
class IEntity
{
public:
    //! \brief IEntity dtor
    virtual ~IEntity (void) = 0;

    //! \brief Called within the event polling phase of the game loop
    //! \details Each entity is responsible for reacting to update events
    //! \param [in] event Polled SDL_Event
    virtual void HandleEvents (const RDGE::Event& event) = 0;

    //! \brief Called within the update phase of the game loop
    //! \details Each entity is responsible for updating their state logic
    //! \param [in] ticks Number of ticks that have occurred since the last update
    virtual void Update (RDGE::UInt32 ticks) = 0;
};

} // namespace GameObjects
} // namespace RDGE
