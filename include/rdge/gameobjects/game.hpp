//! \headerfile <rdge/gameobjects/game.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/config.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/scene.hpp>

#include <SDL.h>

#include <string>
#include <memory>
#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \class Game
//! \brief Base class for a game
//! \details Base class provides a scene stack.  The game loop implementation
//!          is available for use, but functionality for scene manipulation
//!          means it can be moved elsewhere.  Copy and move semantics are
//!          deleted as the intention is to have a single instance, therefore
//!          the Game object should not (and cannot) be derived from IEntity,
//!          although similar methods exist to hook into the game loop.
class Game
{
public:
    //! \brief Game ctor
    //! \details Bootstrap game from immutable settings
    //! \param [in] settings Game settings for bootstrap
    explicit Game (const game_settings& settings);

    //! \brief Game dtor
    virtual ~Game (void);

    //! \brief Game Copy ctor
    //! \details Non-copyable
    Game (const Game&) = delete;

    //! \brief Game Move ctor
    //! \details Non-moveable
    Game (Game&&) = delete;

    //! \brief Game Copy Assignment Operator
    //! \details Non-copyable
    Game& operator= (const Game) = delete;

    //! \brief Game Move Assignment Operator
    //! \details Non-moveable
    Game& operator= (Game&&) = delete;

    //! \brief Get the current scene to execute in the game loop
    //! \details As the scene can potentially change within the
    //!          game loop, this should be called at the start of
    //!          the loop and stored within a local variable.
    //! \returns Shared pointer of the current scene
    std::shared_ptr<Scene> CurrentScene (void) const noexcept;

    //! \brief Run the game loop
    //! \details Processes the three phases of the game loop
    //!          (poll events, update, and render).  A local timer
    //!          passes the delta ticks to perform updates and
    //!          if configured will pass control to the OS on each
    //!          loop iteration to cap the frame rate.  The loop
    //!          can be terminated by setting m_running to false.
    void Run (void);

protected:
    //! \brief Push a new scene on the stack
    //! \param [in] scene Shared pointer of the new scene
    virtual void PushScene (std::shared_ptr<Scene> scene) final;

    //! \brief Pop the current scene of the stack
    virtual void PopScene (void) final;

    //! \brief Process event
    //! \details Calls ProcessEventPhase for the current scene.  Derived
    //!          classes may override but should still call the base
    //!          method for scene processing.
    //! \param [in] event Polled SDL_Event
    virtual void ProcessEventPhase (RDGE::Event& event);

    //! \brief Process update
    //! \details Calls ProcessUpdatePhase for the current scene.  Derived
    //!          classes may override but should still call the base
    //!          method for scene processing.
    //! \param [in] ticks Tick delta from last loop iteration
    virtual void ProcessUpdatePhase (RDGE::UInt32 ticks);

    //! \brief Process render
    //! \details Calls ProcessRenderPhase for the current scene.
    //!          Derived classes may override but should still call
    //!          the base method for scene processing.
    virtual void ProcessRenderPhase (void);

    game_settings                   m_settings;
    std::unique_ptr<RDGE::GLWindow> m_window;
    bool                            m_running;

private:
    //! \typedef SceneStack
    //! \brief Container type for all scenes
    using SceneStack = std::vector<std::shared_ptr<Scene>>;

    SceneStack             m_sceneStack;
    std::shared_ptr<Scene> m_currentScene;
};

} // namespace GameObjects
} // namespace RDGE
