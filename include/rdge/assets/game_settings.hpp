//! \headerfile <rdge/assets/game_settings.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

// TODO This should not be in assets.  This should actually not even be a file.  Move this to
//      application.hpp

#pragma once

#include <rdge/core.hpp>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \struct game_settings
//! \brief Standard layout of settings/configuration options for RDGE
//! \details Can either be populated manually or read from a json file
//!          using the \ref rdge::LoadGameSettings function.
struct game_settings
{
    //! \var enable_jpg Enable to support loading JPG images
    bool enable_jpg   = false;
    //! \var enable_png Enable to support loading PNG images
    bool enable_png   = false;
    //! \var enable_tif Enable to support loading TIF images
    bool enable_tif   = false;
    //! \var enable_fonts Enable to support loading TTF fonts
    bool enable_fonts = false;

    //! \var window_title Title of the game window
    std::string  window_title  = "RDGE";
    //! \var window_icon Path and file for the window icon
    std::string  window_icon   = "";
    //! \var target_width Target drawing size width
    rdge::uint32 target_width  = 960;
    //! \var target_width Target drawing size height
    rdge::uint32 target_height = 540;
    //! \var fullscreen Enable to load the window in full screen mode
    bool         fullscreen    = false;
    //! \var use_vsync Enable vsync (if available)
    bool         use_vsync     = false;
    //! \var target_fps Target frames per second
    rdge::uint8 target_fps     = 60;

    //! \var min_log_level Minimum log level to log
    rdge::uint8 min_log_level  = 2;
};

//! \brief Load and parse the game settings configuration file
//! \details The configuration file must be a json object with the game_settings
//!          variables representing keys.  All fields are optional, and if no file
//!          is found a default game_settings struct will be returned.
//! \param [in] path Configuration file path
//! \returns Populated game_settings struct
//! \throws rdge::Exception Configuration file parsing failed
game_settings
LoadGameSettings (const std::string& path);

} // namespace rdge
