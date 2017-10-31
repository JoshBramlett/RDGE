//! \headerfile <rdge/gameobjects/iscene.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/20/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Event;
//!@}

//! \struct delta_time
//! \brief Container representing a time period
//! \details Passed through the update phase
struct delta_time
{
    uint32 ticks;  //!< Delta time in ticks (milliseconds)
    float seconds; //!< Delta time in seconds

    //! \brief delta_time ctor
    explicit delta_time (uint32 dt)
        : ticks(dt)
        , seconds(static_cast<float>(ticks) / 1000.f)
    { }
};

//! \class IScene
//! \brief Interface for game scenes
//! \details Scenes are containers stored and used by the \ref Game object that
//!          represent an entire game scene, which could be anything from a cut
//!          scene to a splash screen to a battleground.
//!          Scenes basically have three states, which include dormant (not in the
//!          stack), hibernating (suspended in the stack), and active (processing
//!          the game loop).  The state transitions are defined by the interface
//!          methods.  The \ref Game object provides a guarantee that game loop
//!          events will only be sent when the scene is in an active state.
//!          The mapping between methods and states:
//!              - <None>     (Dormant)
//!              - Initialize (Active)
//!              - Terminate  (Dormant)
//!              - Hibernate  (Suspended)
//!              - Activate   (Active)
class IScene
{
public:
    //! \brief IScene dtor
    virtual ~IScene (void) noexcept = default;

    //!@{
    //! \brief Startup and Shutdown
    //! \details Initialization is called when placed on the stack, and conversely
    //!          terminate is called when removed from the stack.  Terminate does not
    //!          imply destruction, but rather it's to signal the scene is no longer
    //!          part of the game hierarchy.  The scene must be prepared to process
    //!          game loop updates after initialization.
    virtual void Initialize (void) = 0;
    virtual void Terminate (void) = 0;
    //!@}

    //!@{
    //! \brief Scene suspension
    //! \details A scene is asked to hibernate when another scene has pushed it lower
    //!          on the stack.  When the other scene has been popped off it will be
    //!          asked to activate and prepare to receive game loop events.
    virtual void Hibernate (void) = 0;
    virtual void Activate (void) = 0;
    //!@}

    //!@{
    //! \brief Game loop processing phases
    //! \details Called only when the scene is active.  In order to receive these events
    //!          the scene must have already had Initialize or Activate called.
    virtual void OnEvent (const Event&) = 0;
    virtual void OnUpdate (const delta_time&) = 0;
    virtual void OnRender (void) = 0;
    //!@}
};

} // namespace rdge
