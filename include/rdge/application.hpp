//! \headerfile <rdge/application.hpp>
//! \author Josh Bramlett
//! \version 0.0.6
//! \date 05/18/2016
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

#include <rdge/types.hpp>
#include <rdge/config.hpp>
#include <rdge/util/logger.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \enum MessageBoxType
//! \brief Dialog type for the message box
//! \details Direct mapping to SDL_MessageBoxFlag, provided for
//!          ease of discovery
enum class MessageBoxType : RDGE::Int32
{
    Info    = SDL_MESSAGEBOX_INFORMATION,
    Warning = SDL_MESSAGEBOX_WARNING,
    Error   = SDL_MESSAGEBOX_ERROR
};

//! \class Application
//! \brief RAII compliant class used for SDL initialization and
//!        destruction.  Also provides numerous helper methods.
//! \details It's required that this class be constructed before
//!          any RDGE objects are instantiated or any SDL calls
//!          are made, and should be the last RDGE object destroyed.
class Application
{
public:
    //! \brief Application ctor
    //! \details Initializes SDL2 library, while optionally
    //!          initializing SDL2 extension libraries.
    //! \param [in] settings Game settings
    //! \throws Initialization failed
    explicit Application (const game_settings& settings);

    //! \brief Application dtor
    //! \details Calls SDL_Quit
    ~Application (void);

    //! \brief Application Copy ctor
    //! \details Non-copyable
    Application (const Application&) = delete;

    //! \brief Application Move ctor
    //! \details Non-moveable
    Application (Application&&) = delete;

    //! \brief Application Copy Assignment Operator
    //! \details Non-copyable
    Application& operator= (const Application) = delete;

    //! \brief Application Move Assignment Operator
    //! \details Non-moveable
    Application& operator= (Application&&) = delete;

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
    //! \throws Failed to get path
    static std::string BasePath (void);

    //! \brief Display native OS message box
    //! \details Should only be called from the thread which created
    //!          the parent window, or the main thread if the parent
    //!          is not provided.  Call will block the thread.
    //! \param [in] type Severity of the message
    //! \param [in] title Message box window title
    //! \param [in] message Content of the message
    //! \param [in] window Optional parent window
    //! \throws Failed to get path
    static void MessageBox (
                            MessageBoxType     type,
                            const std::string& title,
                            const std::string& message,
                            SDL_Window*        window = nullptr
                           );
};

//! \brief Writes an entry to the RDGE log file
//! \details An instance of the Application class must be created prior
//!          to calling this.  If not, the call is a no-op.
//! \param [in] log_level Logging severity
//! \param [in] message Message to write to the log file
//! \param [in] filename Name of the file of the caller (__FILE_NAME__)
//! \param [in] line Line number of the caller (__LINE__)
void
WriteToLogFile (
                RDGE::LogLevel     log_level,
                const std::string& message,
                const std::string& filename = "",
                RDGE::UInt32       line     = 0
               );

//! \brief Writes an entry to the console
//! \details An instance of the Application class must be created prior
//!          to calling this.  If not, the call is a no-op.
//! \param [in] log_level Logging severity
//! \param [in] message Message to write to the log file
//! \param [in] filename Name of the file of the caller (__FILE_NAME__)
//! \param [in] line Line number of the caller (__LINE__)
void
WriteToConsole (
                RDGE::LogLevel     log_level,
                const std::string& message,
                const std::string& filename = "",
                RDGE::UInt32       line     = 0
               );

} // namespace RDGE
