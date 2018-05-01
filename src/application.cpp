#include <rdge/application.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/util/compiler.hpp>

#include <SDL_version.h>
#include <nlohmann/json.hpp>

#include <exception>
#include <sstream>

using json = nlohmann::json;

namespace rdge {

Application::Application (const char* filepath)
    : Application(LoadAppSettings(filepath))
{ }

Application::Application (const app_settings& /* settings */)
{
    // TODO settings file should be for overriding the default log level
    //      (which depends on whether it's a debug or release build).  Therefore
    //      setting a default value in the settings is wrong - this value
    //      should be optional.
    //
    //      Holding off setting the log level from a config until that's done.

    /***********************************************
     *             1.  Initialize SDL
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
    if (RDGE_UNLIKELY(SDL_Init(SDL_INIT_VIDEO) != 0))
    {
        SDL_THROW("SDL Failed to initialize", "SDL_Init");
    }

    /***********************************************
     *          2.  Initialize Logging
     **********************************************/

    InitializeLogger();
}

Application::~Application (void) noexcept
{
    SDL_Quit();
}

/* static */ const std::string&
Application::SDLVersion (void)
{
    // cached for multiple lookups
    static std::string s_version = "";
    if (s_version.empty())
    {
        SDL_version linked;
        SDL_GetVersion(&linked);

        std::ostringstream ss;
        ss << static_cast<int>(linked.major) << "."
           << static_cast<int>(linked.minor) << "."
           << static_cast<int>(linked.patch);

        s_version = ss.str();
    }

    return s_version;
}

/* static */ const std::string&
Application::BasePath (void)
{
    // cached for multiple lookups
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
    if (RDGE_UNLIKELY(SDL_ShowSimpleMessageBox(static_cast<uint32>(type),
                                               title.c_str(),
                                               message.c_str(),
                                               window) != 0))
    {
        RDGE_THROW("Failed to show message box");
    }
}

app_settings
LoadAppSettings (const char* filepath)
{
    app_settings settings;

    try
    {
        auto rwops = rwops_base::from_file(filepath, "rt");
        auto sz = rwops.size();

        std::string file_data(sz + 1, '\0');
        rwops.read(file_data.data(), sizeof(char), sz);

        const auto j = json::parse(file_data);
        if (!j.is_object())
        {
            return settings;
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
