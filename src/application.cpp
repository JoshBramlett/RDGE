#include <rdge/application.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_version.h>

#include <iostream>
#include <sstream>

namespace RDGE {

namespace {

    // cache basepath for multiple lookups
    std::string s_basePath = "";

    // global loggers
    RDGE::Util::FileLogger* s_fileLogger = nullptr;
    RDGE::Util::ConsoleLogger* s_consoleLogger = nullptr;

} // anonymous namespace

Application::Application (const game_settings& settings)
{
    /***********************************************
     *          1.  Initialize Logging
     **********************************************/

    // Validate log level
    auto log_level = static_cast<RDGE::LogLevel>(settings.min_log_level);
    if (log_level < RDGE::LogLevel::Debug || log_level > RDGE::LogLevel::Fatal)
    {
        log_level = RDGE::LogLevel::Warning;
    }

    s_fileLogger = new RDGE::Util::FileLogger("rdge.log", log_level, true);
    s_fileLogger->Write(RDGE::LogLevel::Info, "Built with RDGE v" RDGE_VERSION);

#ifdef RDGE_DEBUG
    s_consoleLogger = new RDGE::Util::ConsoleLogger(RDGE::LogLevel::Debug, true);
#endif

    std::cout << "app log_level=" << static_cast<RDGE::UInt32>(log_level) << std::endl;

    /***********************************************
     *             2.  Initialize SDL
     **********************************************/

    // Valid values include (can be OR'd together):
    // SDL_INIT_TIMER
    // SDL_INIT_AUDIO
    // SDL_INIT_VIDEO
    // SDL_INIT_JOYSTICK
    // SDL_INIT_HAPTIC
    // SDL_INIT_GAMECONTROLLER
    // SDL_INIT_EVENTS
    // SDL_INIT_EVERYTHING
    //
    // Currently the only subsystems used are SDL_INIT_VIDEO and SDL_INIT_EVENTS.
    // This will need to be updated when more functionality is added.

    // Setting to SDL_INIT_VIDEO as it implicitly initializes SDL_INIT_EVENTS as well
    if (UNLIKELY(SDL_Init(SDL_INIT_VIDEO) != 0))
    {
        SDL_THROW("SDL Failed to initialize", "SDL_Init");
    }

    /***********************************************
     *          3.  Initialize SDL_image
     **********************************************/

    RDGE::Int32 image_flags = 0;

    image_flags |= (settings.enable_jpg) ? IMG_INIT_JPG : 0;
    image_flags |= (settings.enable_png) ? IMG_INIT_PNG : 0;
    image_flags |= (settings.enable_tif) ? IMG_INIT_TIF : 0;

    if (UNLIKELY((IMG_Init(image_flags) & image_flags) != image_flags))
    {
        SDL_THROW("SDL_image failed to initialize", "IMG_Init");
    }

    std::cout << "app image_flags=" << image_flags << std::endl;

    /***********************************************
     *          4.  Initialize SDL_ttf
     **********************************************/

    if (settings.enable_fonts)
    {
        if (UNLIKELY(TTF_Init() != 0))
        {
            SDL_THROW("SDL_ttf failed to initialize", "TTF_Init");
        }
    }
}

Application::~Application (void)
{
    if (!s_fileLogger)
    {
        delete s_fileLogger;
    }

    if (!s_consoleLogger)
    {
        delete s_consoleLogger;
    }

    if (TTF_WasInit())
    {
        TTF_Quit();
    }

    IMG_Quit();
    SDL_Quit();
}

std::string
Application::SDLVersion (void) const
{
    SDL_version linked;
    SDL_GetVersion(&linked);

    std::stringstream ss;
    ss << static_cast<RDGE::UInt32>(linked.major) << "."
       << static_cast<RDGE::UInt32>(linked.minor) << "."
       << static_cast<RDGE::UInt32>(linked.patch);

    return ss.str();
}

std::string
Application::SDLImageVersion (void) const
{
    const SDL_version* linked = IMG_Linked_Version();

    std::stringstream ss;
    ss << static_cast<RDGE::UInt32>(linked->major) << "."
       << static_cast<RDGE::UInt32>(linked->minor) << "."
       << static_cast<RDGE::UInt32>(linked->patch);

    return ss.str();
}

std::string
Application::SDLTTFVersion (void) const
{
    const SDL_version* linked = TTF_Linked_Version();

    std::stringstream ss;
    ss << static_cast<RDGE::UInt32>(linked->major) << "."
       << static_cast<RDGE::UInt32>(linked->minor) << "."
       << static_cast<RDGE::UInt32>(linked->patch);

    return ss.str();
}

/* static */ std::string
Application::BasePath (void)
{
    if (s_basePath.empty())
    {
        char *base_path = SDL_GetBasePath();
        if (base_path)
        {
            s_basePath = base_path;
            SDL_free(base_path);
        }
        else
        {
            RDGE_THROW("Failed to retrieve the application path");
        }
    }

    return s_basePath;
}

/* static */ void
Application::MessageBox (
                         MessageBoxType type,
                         const std::string& title,
                         const std::string& message,
                         SDL_Window* window
                        )
{
    if (UNLIKELY(SDL_ShowSimpleMessageBox(
                                          static_cast<RDGE::UInt32>(type),
                                          title.c_str(),
                                          message.c_str(),
                                          window
                                         ) != 0))
    {
        RDGE_THROW("Failed to show message box");
    }
}

void
WriteToLogFile (
                RDGE::LogLevel     log_level,
                const std::string& message,
                const std::string& filename,
                RDGE::UInt32       line
               )
{
    if (!s_fileLogger)
    {
        return;
    }

    s_fileLogger->Write(log_level, message, filename, line);
}

void
WriteToConsole (
                RDGE::LogLevel     log_level,
                const std::string& message,
                const std::string& filename,
                RDGE::UInt32       line
               )
{
    if (!s_consoleLogger)
    {
        return;
    }

    s_consoleLogger->Write(log_level, message, filename, line);
}

} // namespace RDGE
