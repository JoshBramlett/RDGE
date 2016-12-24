#include <rdge/application.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_version.h>
#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>

using namespace rdge;
using json = nlohmann::json;

namespace {

    // global loggers
    rdge::util::FileLogger* s_fileLogger = nullptr;
    rdge::util::ConsoleLogger* s_consoleLogger = nullptr;

} // anonymous namespace

Application::Application (const std::string& path)
    : Application(LoadAppSettings(path))
{ }

Application::Application (const app_settings& settings)
{
    /***********************************************
     *          1.  Initialize Logging
     **********************************************/

    // FIXME RDGE-00054 Refactor app logging

    // Validate log level
    auto log_level = static_cast<LogLevel>(settings.min_log_level);
    if (log_level < LogLevel::DEBUG || log_level > LogLevel::FATAL)
    {
        log_level = LogLevel::WARNING;
    }

    s_fileLogger = new rdge::util::FileLogger("rdge.log", log_level, true);
    s_fileLogger->Write(LogLevel::INFO, "Built with RDGE v" RDGE_VERSION);

#ifdef RDGE_DEBUG
    s_consoleLogger = new rdge::util::ConsoleLogger(LogLevel::DEBUG, true);
#endif

    /***********************************************
     *             2.  Initialize SDL
     **********************************************/

    // FIXME RDGE-00055 Init SDL on demand

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

    int32 image_flags = 0;
    image_flags |= (settings.enable_jpg) ? IMG_INIT_JPG : 0;
    image_flags |= (settings.enable_png) ? IMG_INIT_PNG : 0;
    image_flags |= (settings.enable_tif) ? IMG_INIT_TIF : 0;

    if (UNLIKELY((IMG_Init(image_flags) & image_flags) != image_flags))
    {
        SDL_THROW("SDL_image failed to initialize", "IMG_Init");
    }

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

Application::~Application (void) noexcept
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

    std::ostringstream ss;
    ss << static_cast<int>(linked.major) << "."
       << static_cast<int>(linked.minor) << "."
       << static_cast<int>(linked.patch);

    return ss.str();
}

std::string
Application::SDLImageVersion (void) const
{
    const SDL_version* linked = IMG_Linked_Version();

    std::ostringstream ss;
    ss << static_cast<int>(linked->major) << "."
       << static_cast<int>(linked->minor) << "."
       << static_cast<int>(linked->patch);

    return ss.str();
}

std::string
Application::SDLTTFVersion (void) const
{
    const SDL_version* linked = TTF_Linked_Version();

    std::ostringstream ss;
    ss << static_cast<int>(linked->major) << "."
       << static_cast<int>(linked->minor) << "."
       << static_cast<int>(linked->patch);

    return ss.str();
}

/* static */ const std::string&
Application::BasePath (void)
{
    // cache basepath for multiple lookups
    static std::string s_basePath = "";

    if (s_basePath.empty())
    {
        char* base_path = SDL_GetBasePath();
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
Application::MessageBox (MessageBoxType     type,
                         const std::string& title,
                         const std::string& message,
                         SDL_Window*        window)
{
    if (UNLIKELY(SDL_ShowSimpleMessageBox(
                                          static_cast<uint32>(type),
                                          title.c_str(),
                                          message.c_str(),
                                          window
                                         ) != 0))
    {
        RDGE_THROW("Failed to show message box");
    }
}

app_settings
rdge::LoadAppSettings (const std::string& path)
{
    app_settings settings;

    try
    {
        auto config = rdge::util::read_text_file(path.c_str());
        if (config.empty())
        {
            return settings;
        }

        auto j = json::parse(config);
        if (!j.is_object())
        {
            return settings;
        }

        if (j["enable_jpg"].is_boolean())
        {
            settings.enable_jpg = j["enable_jpg"];
        }

        if (j["enable_png"].is_boolean())
        {
            settings.enable_png = j["enable_png"];
        }

        if (j["enable_tif"].is_boolean())
        {
            settings.enable_tif = j["enable_tif"];
        }

        if (j["enable_fonts"].is_boolean())
        {
            settings.enable_fonts = j["enable_fonts"];
        }

        if (j["window_title"].is_string())
        {
            settings.window_title = j["window_title"];
        }

        if (j["window_icon"].is_string())
        {
            settings.window_icon = j["window_icon"];
        }

        if (j["target_width"].is_number())
        {
            settings.target_width = j["target_width"];
        }

        if (j["target_height"].is_number())
        {
            settings.target_height = j["target_height"];
        }

        if (j["fullscreen"].is_boolean())
        {
            settings.fullscreen = j["fullscreen"];
        }

        if (j["resizable"].is_boolean())
        {
            settings.resizable = j["resizable"];
        }

        if (j["use_vsync"].is_boolean())
        {
            settings.use_vsync = j["use_vsync"];
        }

        if (j["target_fps"].is_number())
        {
            settings.target_fps = j["target_fps"];
        }

        if (j["min_log_level"].is_number())
        {
            settings.min_log_level = j["min_log_level"];
        }
    }
    catch (const std::domain_error& ex)
    {
        RDGE_THROW("Domain error parsing config file! error=" + std::string(ex.what()));
    }
    catch (const std::out_of_range& ex)
    {
        RDGE_THROW("Out of range error parsing config file! error=" + std::string(ex.what()));
    }

    return settings;
}

void
rdge::WriteToLogFile (LogLevel           log_level,
                      const std::string& message,
                      const std::string& filename,
                      uint32             line)
{
    if (s_fileLogger)
    {
        s_fileLogger->Write(log_level, message, filename, line);
    }
}

void
rdge::WriteToConsole (LogLevel           log_level,
                      const std::string& message,
                      const std::string& filename,
                      uint32             line)
{
    if (s_consoleLogger)
    {
        s_consoleLogger->Write(log_level, message, filename, line);
    }
}
