//! \headerfile <rdge/application.hpp>
//! \author Josh Bramlett
//! \version 0.0.6
//! \date 05/18/2016

#pragma once

#include <rdge/types.hpp>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \struct game_settings
//! \brief POD collection of settings/configuration options for RDGE
//! \details Can either be populated manually or read from a json file
//!          using the ReadConfigFile function.  The format of the json
//!          file is a top level json object using the game_settings
//!          variables as keys.
struct game_settings
{
    //! \var enable_jpg Enable to support loading JPG images
    bool enable_jpg;
    //! \var enable_png Enable to support loading PNG images
    bool enable_png;
    //! \var enable_tif Enable to support loading TIF images
    bool enable_tif;

    //! \var enable_fonts Enable to support loading TTF fonts
    bool enable_fonts;

    //! \var window_title Title of the game window
    std::string  window_title;
    //! \var window_icon Path and file for the window icon
    std::string  window_icon;
    //! \var target_width Target drawing size width
    RDGE::UInt32 target_width;
    //! \var target_width Target drawing size height
    RDGE::UInt32 target_height;
    //! \var fullscreen Enable to load the window in full screen mode
    bool         fullscreen;
    //! \var use_vsync Enable vsync (if available)
    bool         use_vsync;
    //! \var target_fps Target frames per second
    RDGE::UInt32 target_fps;

    //! \var min_log_level Minimum log level to log
    RDGE::UInt32 min_log_level;

    //! \brief game_settings ctor
    //! \details Initializes all settings to their default values
    game_settings (void)
        : enable_jpg(false)
        , enable_png(false)
        , enable_tif(false)
        , enable_fonts(false)
        , window_title("RDGE")
        , window_icon("")
        , target_width(960)
        , target_height(540)
        , fullscreen(false)
        , use_vsync(false)
        , target_fps(60)
        , min_log_level(2)
    { }
};

//! \brief Read configuration file
//! \details The configuration file must be a json object with the game_settings
//!          variables representing keys.  All fields are optional, and if no file
//!          is found a default game_settings struct will be returned.
//! \param [in] file Configuration file and path
//! \returns game_settings struct
//! \throws RDGE::Exception Configuration file parsing failed
game_settings
ReadConfigFile (const std::string& file);

} // namespace RDGE
