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
//! \brief Global Debug Settings
//! \details Allows programmatic access to flags used by ImGui debug overlay
namespace settings {

extern bool show_overlay; //!< Show/Hide global debug overlay

//!@{ Scene main menu items
extern bool show_camera_widget;     //!< Show/Hide Camera Widget
extern bool show_physics_widget;    //!< Show/Hide Physics Widget
extern bool show_graphics_widget;   //!< Show/Hide Graphics Widget
//!@}

//!@{ Memory main menu items
extern bool show_memory_tracker;    //!< Show/Hide Memory Tracker Widget
//!@}

//!@{ ImGui main menu items
extern bool show_imgui_test_window; //!< Show/Hide ImGui 'Test Window'
//!@}

//!@{ Camera Widget
extern bool draw_camera_viewport;   //!< Draw viewport boundary
//!@}

//!@{ Physics Widget Properties
extern bool draw_physics_fixtures;       //!< Draw Fixture shapes
extern bool draw_physics_proxy_aabbs;    //!< Draw BVH Proxy AABBs
extern bool draw_physics_joints;         //!< Draw Joints
extern bool draw_physics_center_of_mass; //!< Draw RigidBody center of mass
extern bool draw_physics_bvh_nodes;      //!< Draw Bounding Volume Hierarchy nodes
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
