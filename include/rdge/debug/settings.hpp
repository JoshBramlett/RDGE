//! \headerfile <rdge/debug/settings.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/12/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \struct scene_widget_settings
//! \brief Provided to the scene to render ImGui widgets
//! \details Values map directly to the "Scene" menu in the main menu bar.  Scenes
//!          are directed to call the Debug_UpdateWidget of the appropriate module
//!          which will build the custom ImGui widget.
struct scene_widget_settings
{
    bool show_camera_widget = false;   //!< Show OrthographicCamera widget
    bool show_physics_widget = false;  //!< Show CollisionGraph widget
    bool show_graphics_widget = false;
};

} // namespace debug
} // namespace rdge
