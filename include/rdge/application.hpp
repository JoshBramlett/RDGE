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
//!          game engine.
//! \li GatorQue Engine
//!     The first homebrew game engine I dissected to learn it's internals
//!     \see http://code.google.com/p/gqe/
//! \li Twinklebear's resources
//!     Easy to understand tutorials for getting started with CMake and SDL
//!     \see http://www.willusher.io/pages/sdl2/
//!     \see https://github.com/Twinklebear/LPCGame
//! \li C++11 bindings/wrapper for SDL2
//!     Good resource for wrapping SDL2 in an RAII friendly manner
//!     \see https://github.com/libSDL2pp/libSDL2pp
//! \li Simple C++11 friendly header-only bindings to Lua
//!     Comes with a great tutorial for creating Lua bindings
//!     \see https://github.com/jeremyong/Selene

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/logger.hpp>

#include <SDL.h>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \struct app_settings
//! \brief Standard layout of settings/configuration options for RDGE
//! \details Can either be populated manually or read from a json file
//!          using the \ref rdge::LoadAppSettings function.
struct app_settings
{
    bool enable_jpg   = false; //!< Enable support for loading JPG images
    bool enable_png   = false; //!< Enable support for loading PNG images
    bool enable_tif   = false; //!< Enable support for loading TIF images
    bool enable_fonts = false; //!< Enable support for loading TTF fonts

    std::string window_title  = "RDGE"; //!< Title of the game window
    std::string window_icon   = "";     //!< Path and file for the window icon
    uint32      target_width  = 960;    //!< Target drawing size width
    uint32      target_height = 540;    //!< Target drawing size height
    bool        fullscreen    = false;  //!< Load the window in full screen mode
    bool        resizable     = false;  //!< Allow window to be resized
    bool        use_vsync     = false;  //!< Enable vsync (if available)
    uint32      target_fps    = 60;     //!< Target frames per second (ignored if use_vsync enabled)

    uint32 min_log_level  = 2; //!< Minimum log level
};

//! \enum MessageBoxType
//! \brief Dialog type for the message box
//! \details Direct mapping to SDL_MessageBoxFlag, provided for
//!          abstraction and ease of discovery.
enum class MessageBoxType : rdge::int32
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
    //! \param [in] path Path to settings config file
    //! \throws rdge::Exception Configuration file parsing failed
    //! \throws rdge::SDLException SDL Initialization failed
    explicit Application (const std::string& path);

    //! \brief Application dtor
    //! \details Calls SDL_Quit
    ~Application (void) noexcept;

    //!@{
    //! \details Non-copyable and non-movable
    Application (const Application&) = delete;
    Application& operator= (const Application&) = delete;
    Application (Application&&) = delete;
    Application& operator= (Application&&) = delete;
    //!@}

    //! \brief SDL version linked against
    //! \return String containing the major, minor, and patch
    std::string SDLVersion (void) const;

    //! \brief SDL_image version linked against
    //! \return String containing the major, minor, and patch
    std::string SDLImageVersion (void) const;

    //! \brief SDL_ttf version linked against
    //! \return String containing the major, minor, and patch
    std::string SDLTTFVersion (void) const;

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
//!          is found a default game_settings struct will be returned.
//! \param [in] path Configuration file path
//! \returns Populated app_settings struct
//! \throws rdge::Exception Configuration file parsing failed
app_settings
LoadAppSettings (const std::string& path);

//! \brief Writes an entry to the RDGE log file
//! \param [in] log_level Logging severity
//! \param [in] message Message to write to the log file
//! \param [in] filename Name of the file of the caller (FILE_NAME)
//! \param [in] line Line number of the caller (__LINE__)
void
WriteToLogFile (rdge::LogLevel     log_level,
                const std::string& message,
                const std::string& filename = "",
                rdge::uint32       line     = 0);

//! \brief Writes an entry to the console
//! \details Only enabled in debug builds.
//! \param [in] log_level Logging severity
//! \param [in] message Message to write to the log file
//! \param [in] filename Name of the file of the caller (FILE_NAME)
//! \param [in] line Line number of the caller (__LINE__)
void
WriteToConsole (rdge::LogLevel     log_level,
                const std::string& message,
                const std::string& filename = "",
                rdge::uint32       line     = 0);

} // namespace rdge
