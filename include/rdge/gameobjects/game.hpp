//! \headerfile <rdge/gameobjects/game.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/20/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/system/window.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>

#include <functional>
#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Game
//! \brief Game loop and scene stack
//! \details The Game will create a window and run a game loop in accordance with
//!          the provided \ref game_settings.  Maintains a stack of \ref Scene
//!          objects with the topmost being the current scene that will receive
//!          the game loop events.
class Game
{
public:
    //!@{
    //! \brief Game loop event hooking callbacks
    //! \details The result of each hook signifies whether the game loop will
    //!          suppress (true) or forward (false) the event to the current scene.
    using OnEventCallback  = std::function<bool(const Event&)>;
    using OnUpdateCallback = std::function<bool(const delta_time&)>;
    using OnRenderCallback = std::function<bool(void)>;
    //!@}

    //! \brief Game ctor
    //! \details Bootstrap game from settings
    //! \param [in] settings App settings
    explicit Game (const app_settings& settings);

    //! \brief Game dtor
    ~Game (void) noexcept;

    //!@{
    //! \brief Copy and move disabled
    Game (const Game&) = delete;
    Game (Game&&) = delete;
    Game& operator= (const Game) = delete;
    Game& operator= (Game&&) = delete;
    //!@}

    //! \brief Push a new scene on the stack
    //! \param [in] scene Shared pointer of the new scene
    void PushScene (std::shared_ptr<IScene> scene);

    //! \brief Pop the current scene of the stack
    void PopScene (void);

    //! \brief Run the game loop
    //! \details Game loop is broken down to three phases which includes event
    //!          polling, time delta updating, and rendering.  The events are
    //!          invoked on the current scene for further processing.  If vsync
    //!          is not defined or not available, the loop will yield to the OS
    //!          for any time remaining in the loop to accomodate the target FPS.
    //!          The loop will terminate when instructed to or there is no scene
    //!          available on the stack.
    void Run (void);

    //! \brief Stop the game loop
    void Stop (void);

public:
    app_settings            settings; //!< Game settings
    std::unique_ptr<Window> window;   //!< Game window

    OnEventCallback  on_event_hook;  //!< OnEvent hook function pointer
    OnUpdateCallback on_update_hook; //!< OnUpdate hook function pointer
    OnRenderCallback on_render_hook; //!< OnRender hook function pointer

private:
    std::vector<std::shared_ptr<IScene>> m_sceneStack; //!< Scene stack

    bool m_running      = false; //!< Flag for running the game loop
    bool m_pushDeferred = false; //!< Scene push deferred until loop iteration completes
    bool m_popDeferred  = false; //!< Scene pop deferred until loop iteration completes
};

} // namespace rdge
