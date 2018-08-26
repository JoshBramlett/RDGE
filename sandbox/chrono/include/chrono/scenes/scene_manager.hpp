#pragma once

#include <chrono/types.hpp>

namespace perch {

//!@{ Scene management
void PushScene (chrono_scene_id scene_id);
void PopScene (void);
void SwapScene (chrono_scene_id scene_id);

void DestroyAllScenes (void);
//!@}

} // namespace perch
