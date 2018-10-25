//! \headerfile <rdge/debug/renderer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/02/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/graphics/color.hpp>

//!@{ Forward declarations
struct SDL_Window;

namespace rdge {
class OrthographicCamera;
class SpriteLayer;
class TileLayer;
class Event;
struct delta_time;

namespace debug {
class IWidget;
}

namespace math {
struct vec3;
struct mat4;
}

namespace physics {
class CollisionGraph;
class Fixture;
struct ishape;
struct aabb;
struct circle;
struct polygon;
}

}
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

#ifdef RDGE_DEBUG

static const rdge::color DEFAULT_COLOR = color::YELLOW; //!< Default line color

//! \namespace settings
//! \brief Global ImGui debug overlay settings
namespace settings {

extern bool show_overlay;           //!< Show/Hide global debug overlay
extern bool show_imgui_test_window; //!< Show/Hide ImGui 'Test Window'

//!@{ Camera Widget Properties
namespace camera {
    extern bool show_widget;
    extern rdge::OrthographicCamera* active_camera;

    // widget flags
    extern bool draw_viewport;

    // default wireframe colors
    namespace colors {
        extern rdge::color viewport;
    }
}
//!@}

//!@{ Graphics Widget Properties
namespace graphics {
    extern bool show_widget;
    extern std::vector<rdge::SpriteLayer*> active_sprite_layers;
    extern std::vector<rdge::TileLayer*> active_tile_layers;

    // widget flags
    extern bool hide_all_layers;

    // default wireframe colors
    namespace colors {
        extern rdge::color sprites;
        extern rdge::color tile_region;
    }
}
//!@}

//!@{ Physics Widget Properties
namespace physics {
    extern bool show_widget;
    extern rdge::physics::CollisionGraph* active_graph;
    extern float scale;

    // widget flags
    extern bool draw_fixtures;
    extern bool draw_proxy_aabbs;
    extern bool draw_joints;
    extern bool draw_center_of_mass;
    extern bool draw_bvh_nodes;

    // default wireframe colors
    namespace colors {
        extern rdge::color not_simulating;
        extern rdge::color static_body;
        extern rdge::color kinematic_body;
        extern rdge::color sleeping_body;
        extern rdge::color dynamic_body;
        extern rdge::color proxy_aabb;
        extern rdge::color center_of_mass;
        extern rdge::color joints;
    }
}
//!@}

//!@{ Memory Widget Properties
namespace memory {
    extern bool show_widget;
}
//!@}

} // namespace settings

//!@{
//! \brief Game loop support for stateless debug rendering
//! \details Support implemented in the \ref Game object.  Do not call directly.
void InitializeOverlay (void);
bool ProcessOnEvent (Event&);
void ProcessOnUpdate (SDL_Window* window, const delta_time&);
void ProcessOnRender (void);
//!@}

//!@{ Set properties impacting the global debug renderer state
void SetLineWidth (float width);
void SetProjection (const math::mat4& projection);
//!@}

//!@{ Registers objects with the overlay dialogs
void AddWidget (IWidget* widget);
void RemoveWidget (IWidget* widget);
void RegisterCamera (OrthographicCamera* camera);
void RegisterPhysics (physics::CollisionGraph* graph, float scale = 1.f);
void RegisterGraphics (SpriteLayer* layer);
void RegisterGraphics (TileLayer* layer);
void ClearGraphics (void);
//!@}

//!@{ Cache/Load debug overlay settings
uint32 CacheSettings (void);
void LoadSettings (uint32 flags);
//!@}

//!@{ Draw primatives
void DrawPoint (const math::vec2& pos, const color& c = DEFAULT_COLOR, float size = 1.f);
void DrawLine (const math::vec2& pa, const math::vec2& pb, const color& c = DEFAULT_COLOR);
//!@}

//!@{ Draw wireframe for physics objects (scale is the pixels per meter)
void DrawWireFrame (const physics::aabb&, const color& = DEFAULT_COLOR, float scale = 1.f);
void DrawWireFrame (const physics::circle&, const color& = DEFAULT_COLOR, float scale = 1.f);
void DrawWireFrame (const physics::polygon&, const color& = DEFAULT_COLOR, float scale = 1.f);
void DrawWireFrame (const physics::ishape*, const color& = DEFAULT_COLOR, float scale = 1.f);
void DrawWireFrame (const physics::Fixture*, const color& = DEFAULT_COLOR, float scale = 1.f);
//!@}

#else
inline void InitializeRenderer (void) { }
inline void FlushRenderer (void) { }
inline void SetLineWidth (float) { }
inline void SetProjection (const math::mat4&) { }
inline void DrawLine (const math::vec3&, const math::vec3&, const color&) { }

template <typename T>
inline void DrawWireFrame (const T&, const color& = DEFAULT_COLOR, float = 0.f) { }
#endif

} // namespace debug
} // namespace rdge
