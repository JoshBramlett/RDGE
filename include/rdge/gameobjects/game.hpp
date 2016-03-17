//! \headerfile <rdge/gameobjects/game.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/30/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/scene.hpp>

#include <SDL.h>

#include <string>
#include <memory>
#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \struct GameSettings
//! \brief Settings required for game bootstrap
//! \details Settings are read during \ref Game instantiation and are
//!          immutable.  Generally populated via config file.
struct GameSettings
{
    std::string  name;
    std::string  window_title;
    RDGE::UInt32 target_fps;
    bool         use_vsync;
    std::string  icon;
    RDGE::UInt32 target_width;
    RDGE::UInt32 target_height;
    bool         fullscreen;
};

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
    explicit Game (const GameSettings& settings);

    //! \brief Game dtor
    virtual ~Game (void) { }

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
    //! \details Calls HandleEvents for the current scene.  Derived
    //!          classes may override but should still call the base
    //!          method for scene processing.
    //! \param [in] event Polled SDL_Event
    virtual void ProcessEvent (const SDL_Event& event);

    //! \brief Process update
    //! \details Calls Update for the current scene.  Derived classes
    //!          may override but should still call the base method
    //!          for scene processing.
    //! \param [in] ticks Tick delta from last loop iteration
    virtual void ProcessUpdate (RDGE::UInt32 ticks);

    //! \brief Process render
    //! \details Calls Render for the current scene.  Derived classes
    //!          may override but should still call the base method
    //!          for scene processing.
    //! \param [in] window Game window
    virtual void ProcessRender (const RDGE::Window& window);

    GameSettings m_settings;
    RDGE::Window m_window;
    bool         m_running;

private:
    //! \typedef SceneStack
    //! \brief Container type for all scenes
    using SceneStack = std::vector<std::shared_ptr<Scene>>;

    SceneStack             m_sceneStack;
    std::shared_ptr<Scene> m_currentScene;
};

} // namespace GameObjects
} // namespace RDGE
