#include <rdge/config.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <nlohmann/json.hpp>

#include <exception>

namespace RDGE {

using json = nlohmann::json;

game_settings
ReadConfigFile (const std::string& file)
{
    game_settings settings;

    auto config = RDGE::Util::read_text_file(file.c_str());
    if (config.empty())
    {
        return settings;
    }

    try
    {
        auto j = json::parse(config);

        if (j["enable_jpg"] != nullptr && j["enable_jpg"].is_boolean())
        {
            settings.enable_jpg = j["enable_jpg"];
        }

        if (j["enable_png"] != nullptr && j["enable_png"].is_boolean())
        {
            settings.enable_png = j["enable_png"];
        }

        if (j["enable_tif"] != nullptr && j["enable_tif"].is_boolean())
        {
            settings.enable_tif = j["enable_tif"];
        }

        if (j["enable_fonts"] != nullptr && j["enable_fonts"].is_boolean())
        {
            settings.enable_fonts = j["enable_fonts"];
        }

        if (j["window_title"] != nullptr && j["window_title"].is_string())
        {
            settings.window_title = j["window_title"];
        }

        if (j["window_icon"] != nullptr && j["window_icon"].is_string())
        {
            settings.window_icon = j["window_icon"];
        }

        if (j["target_width"] != nullptr && j["target_width"].is_number())
        {
            settings.target_width = j["target_width"];
        }

        if (j["target_height"] != nullptr && j["target_height"].is_number())
        {
            settings.target_height = j["target_height"];
        }

        if (j["fullscreen"] != nullptr && j["fullscreen"].is_boolean())
        {
            settings.fullscreen = j["fullscreen"];
        }

        if (j["use_vsync"] != nullptr && j["use_vsync"].is_boolean())
        {
            settings.use_vsync = j["use_vsync"];
        }

        if (j["target_fps"] != nullptr && j["target_fps"].is_number())
        {
            settings.target_fps = j["target_fps"];
        }

        if (j["min_log_level"] != nullptr && j["min_log_level"].is_number())
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

} // namespace RDGE
