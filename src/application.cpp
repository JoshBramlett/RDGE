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

Application::Application (
                          RDGE::UInt32 flags,
                          RDGE::UInt32 img_flags,
                          bool         init_sdl_ttf,
                          bool         init_sdl_mixer,
                          bool         init_sdl_net
                         )
{
    // TODO: LogLevel should be from config file
    s_fileLogger = new RDGE::Util::FileLogger(
                                          "rdge.log",
                                          RDGE::LogLevel::Debug,
                                          true
                                         );

    s_consoleLogger = new RDGE::Util::ConsoleLogger(
                                          RDGE::LogLevel::Debug,
                                          true
                                         );

    s_fileLogger->Write(RDGE::LogLevel::Info, "Built with RDGE v" RDGE_VERSION);

    if (UNLIKELY(SDL_Init(flags) != 0))
    {
        SDL_THROW("SDL Failed to initialize", "SDL_Init");
    }

    if (UNLIKELY((IMG_Init(img_flags) & img_flags) != img_flags))
    {
        SDL_THROW("SDL_image failed to initialize", "IMG_Init");
    }

    if (init_sdl_ttf)
    {
        if (UNLIKELY(TTF_Init() != 0))
        {
            SDL_THROW("SDL_ttf failed to initialize", "TTF_Init");
        }
    }

    if (init_sdl_mixer)
    {
        // TODO Implement
    }

    if (init_sdl_net)
    {
        // TODO Implement
    }
}

Application::~Application (void)
{
    if (!s_fileLogger)
    {
        delete s_fileLogger;
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
        std::cerr << "ERROR:  Attempting to use file logger before the RDGE::Application "
                  << "has been instantiated." << std::endl;

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
        std::cerr << "ERROR:  Attempting to use console logger before the RDGE::Application "
                  << "has been instantiated." << std::endl;

        return;
    }

    s_consoleLogger->Write(log_level, message, filename, line);
}

} // namespace RDGE
