//! \headerfile <rdge/application.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/17/2016
//!
//! \details This project never would have made it off the ground had
//!          it not been for the numerous resources available on the web.
//!          RDGE was 100% written from scratch, but contains elements
//!          from many published projects that provided inspiration.
//!          Thanks to everyone listed below whose examples have given
//!          me a better understanding of how to go about creating this
//!          game engine.  These are all in chronological order of discovery.
//! \li GatorQue Engine
//!     The first homebrew game engine I dissected to learn it's internals
//!     \see http://code.google.com/p/gqe/
//! \li Twinklebear's resources
//!     Easy to understand tutorials for getting started with CMake and SDL
//!     \see http://www.willusher.io/pages/sdl2/
//!     \see https://github.com/Twinklebear/LPCGame
//! \li libSDL2pp - C++11 bindings/wrapper for SDL2
//!     Good resource for wrapping SDL2 in an RAII friendly manner
//!     \see https://github.com/libSDL2pp/libSDL2pp
//! \li LibGDX
//!     Open source Java game engine
//! \li Sparky Game Engine
//!     Decent videos for constructing a basic game engine using OpenGL
//! \li Handmade Hero
//!     Invaluable resource.  A game/game engine written from scratch
//! \li Box2D
//!     Fantastic open source 2D rigid body physics engine

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/logger.hpp>

#include <SDL.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct app_settings
//! \brief Standard layout of settings/configuration options for RDGE
//! \details Can either be populated manually or read from a json file
//!          using the \ref rdge::LoadAppSettings function.
struct app_settings
{
    std::string window_title  = "RDGE"; //!< Title of the game window
    std::string window_icon   = "";     //!< Path and file for the window icon
    uint32      target_width  = 960;    //!< Target drawing size width
    uint32      target_height = 540;    //!< Target drawing size height
    bool        fullscreen    = false;  //!< Load the window in full screen mode
    bool        resizable     = false;  //!< Allow window to be resized
    bool        use_vsync     = true;   //!< Enable vsync (if available)
    uint32      target_fps    = 60;     //!< Target frames per second (ignored if use_vsync enabled)

    uint32 min_log_level  = 2; //!< Minimum log level
};

//! \enum MessageBoxType
//! \brief Dialog type for the message box
//! \details Direct mapping to SDL_MessageBoxFlag, provided for
//!          abstraction and ease of discovery.
enum class MessageBoxType : int32
{
    INFO    = SDL_MESSAGEBOX_INFORMATION,
    WARNING = SDL_MESSAGEBOX_WARNING,
    ERROR   = SDL_MESSAGEBOX_ERROR
};

//! \class Application
//! \brief Performs pre-initialization and provides utility methods
//! \details If this class is instantiated, there should only be one
//!          instance throughout the entire application and it should
//!          be within scope throughout the entire lifetime.  Premature
//!          destruction is likely to crash the application.
//! \note It's highly recommended this class is utilized.
class Application
{
public:
    //! \brief Application ctor
    //! \details Initializes the SDL2 libraries and application logging
    //!          depending on the provided app settings.
    //! \param [in] settings App settings
    //! \throws rdge::SDLException SDL Initialization failed
    explicit Application (const app_settings& settings);

    //! \brief Application ctor
    //! \details Loads app settings from the provided configuration file
    //!          path and performs all initialization.
    //! \param [in] filepath Path to settings config file
    //! \throws rdge::Exception Configuration file parsing failed
    //! \throws rdge::SDLException SDL Initialization failed
    explicit Application (const char* filepath);

    //! \brief Application dtor
    //! \details Calls SDL_Quit
    ~Application (void) noexcept;

    //!@{ Non-copyable and non-movable
    Application (const Application&) = delete;
    Application& operator= (const Application&) = delete;
    Application (Application&&) = delete;
    Application& operator= (Application&&) = delete;
    //!@}

    //!@{ Version queries
    static const std::string& Version (void);
    static const std::string& SDLVersion (void);
    //!@}

    //! \brief Application base directory path
    //! \return Directory path where application resides
    //! \throws rdge::SDLException Failed to get path
    static const std::string& BasePath (void);

    //! \brief Display native OS message box
    //! \details Call will block the thread.  If a parent window is
    //!          specified, this should only be called on the thread
    //!          which created the window.
    //! \param [in] type Severity of the message
    //! \param [in] title Message box window title
    //! \param [in] message Content of the message
    //! \param [in] window Optional parent window
    //! \throws rdge::Exception Failed to get path
    static void MessageBox (MessageBoxType     type,
                            const std::string& title,
                            const std::string& message,
                            SDL_Window*        window = nullptr);
};

//! \brief Load and parse the app settings configuration file
//! \details The configuration file must be a json object with the app_settings
//!          variables representing keys.  All fields are optional, and if no file
//!          is found a default \ref app_settings struct will be returned.
//! \param [in] filepath Configuration file path
//! \returns Populated app_settings struct
//! \throws rdge::Exception Configuration file parsing failed
app_settings
LoadAppSettings (const char* filepath);

} // namespace rdge
