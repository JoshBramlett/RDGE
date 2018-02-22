//! \headerfile <rdge/debug/renderer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/02/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/color.hpp>

//!@{ Forward declarations
struct SDL_Window;

namespace rdge {
class OrthographicCamera;
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

extern bool show_overlay; //!< Show/Hide global debug overlay

//!@{ Scene main menu items
extern bool show_camera_widget;     //!< Show/Hide Camera Widget
extern bool show_graphics_widget;   //!< Show/Hide Graphics Widget
//!@}

//!@{ Memory main menu items
namespace memory {
    // widget display
    extern bool show_widget;
} // namespace memory
//!@}

//!@{ ImGui main menu items
extern bool show_imgui_test_window; //!< Show/Hide ImGui 'Test Window'
//!@}

//!@{ Camera Widget
extern bool draw_camera_viewport;   //!< Draw viewport boundary
//!@}

    //!@{ Physics Widget Properties
    namespace physics {
        // widget display
        extern bool show_widget;

        // widget flags
        extern bool draw_fixtures;
        extern bool draw_proxy_aabbs;
        extern bool draw_joints;
        extern bool draw_center_of_mass;
        extern bool draw_bvh_nodes;

        // default wireframe colors
        namespace colors {
            extern color not_simulating;
            extern color static_body;
            extern color kinematic_body;
            extern color sleeping_body;
            extern color dynamic_body;
            extern color proxy_aabb;
            extern color center_of_mass;
            extern color joints;
        } // namespace colors
    } // namespace physics
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
void RegisterCamera (OrthographicCamera* camera);
void RegisterPhysics (physics::CollisionGraph* graph, float scale = 1.f);
//!@}

//!@{ Draw primatives
void DrawPoint (const math::vec2& pos, const color& c = DEFAULT_COLOR, float size = 1.f);
void DrawLine (const math::vec2& pa, const math::vec2& pb, const color& c = DEFAULT_COLOR);
//!@}

//!@{ Draw wireframe
void DrawWireFrame (const SpriteVertices& vertices, const color& c = DEFAULT_COLOR);
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
