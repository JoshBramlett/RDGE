#include <rdge/application.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_ttf.h>
#include <SDL_version.h>
#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>

using json = nlohmann::json;

namespace rdge {

Application::Application (const std::string& path)
    : Application(LoadAppSettings(path))
{ }

Application::Application (const app_settings& settings)
{
    /***********************************************
     *          1.  Initialize Logging
     **********************************************/

    InitializeLogger();

    // TODO settings file should be for overriding the default log level
    //      (which depends on whether it's a debug or release build).  Therefore
    //      setting a default value in the settings is wrong - this value
    //      should be optional.
    //
    //      Holding off setting the log level from a config until that's done.

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
    if (TTF_WasInit())
    {
        TTF_Quit();
    }

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
    if (UNLIKELY(SDL_ShowSimpleMessageBox(static_cast<uint32>(type),
                                          title.c_str(),
                                          message.c_str(),
                                          window) != 0))
    {
        RDGE_THROW("Failed to show message box");
    }
}

app_settings
LoadAppSettings (const std::string& path)
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

} // namespace rdge
