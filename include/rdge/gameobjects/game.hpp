//! \headerfile <rdge/gameobjects/game.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/20/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/application.hpp>

#include <functional>
#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Event;
class Window;
class IScene;
struct delta_time;
//!@}

//! \class Game
//! \brief Game loop and scene stack
//! \details The Game will create a window and run a game loop in accordance with
//!          the provided \ref app_settings.  Maintains a stack of \ref Scene
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

    //!@{ Non-copyable, Non-movable
    Game (const Game&) = delete;
    Game (Game&&) = delete;
    Game& operator= (const Game) = delete;
    Game& operator= (Game&&) = delete;
    //!@}

    //! \brief Push a new scene on the stack
    //! \param [in] scene New active scene
    void PushScene (std::shared_ptr<IScene> scene);

    //! \brief Pop the current scene of the stack
    void PopScene (void);

    //! \brief Replace the current scene
    //! \param [in] scene New active scene
    void SwapScene (std::shared_ptr<IScene> scene);

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

    enum StateFlags
    {
        RUNNING       = 0x0001, //!< Flag for running the game loop
        PUSH_DEFERRED = 0x0002, //!< Scene push deferred until loop iteration completes
        POP_DEFERRED  = 0x0004, //!< Scene pop deferred until loop iteration completes
        SWAP_DEFERRED = 0x0008, //!< Scene swap deferred until loop iteration completes
        ANY_DEFERRED  = PUSH_DEFERRED | POP_DEFERRED | SWAP_DEFERRED
    };

    uint8 m_flags = 0;
};

} // namespace rdge
