#pragma once

#include <chrono/types.hpp>

//!@{ Forward declarations
namespace rdge {
class CustomEventArgs;
} // namespace rdge
//!@}

namespace perch {

//! \brief Game event handling setup
//! \details Disables unused events and sets up the custom event handling.  This
//!          must be callled prior to running the game.
void SetupEvents (void);

//!@{ Custom Events
void EnqueueScenePushEvent (chrono_scene_id scene_id);
void EnqueueScenePopEvent (void);
bool ProcessCustomEvent (const rdge::CustomEventArgs& args);
//!@}

} // namespace perch
