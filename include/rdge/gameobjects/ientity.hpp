//! \headerfile <rdge/gameobjects/ientity.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/29/2015
//! \bug

#pragma once

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/window.hpp>

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
    virtual void HandleEvents (const SDL_Event& event) = 0;

    //! \brief Called within the update phase of the game loop
    //! \details Each entity is responsible for updating their state logic
    //! \param [in] ticks Number of ticks that have occurred since the last update
    virtual void Update (RDGE::UInt32 ticks) = 0;

    //! \brief Called within the drawing phase of the game loop
    //! \details Each entity is responsible for drawing themselves to the renderer
    //! \param [in] window Window object to draw to
    virtual void Render (const RDGE::Window& window) = 0;

    //! \brief Identifier used to link Entity objects
    //! \details Allows logical linkage of objects (e.g. enemies, npcs, etc.)
    //! \returns String tag
    virtual std::string Tag (void) const = 0;
};

} // namespace GameObjects
} // namespace RDGE
