//! \headerfile <rdge/debug/renderer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/02/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

namespace physics {

class Fixture;
struct aabb;
struct circle;
struct polygon;

}

namespace debug {

#ifdef RDGE_DEBUG

static const rdge::color DEFAULT_COLOR = color::YELLOW; //!< Default line color

//!@{
//! \brief Stateless debug rendering
//! \details Initialize and flush calls are performed automatically by the
//!          \ref Game object.  If a different capacity is required than
//!          the default InitializeRenderer should be called between the
//!          Game object creation and the running of the game loop.
void InitializeRenderer (uint16 capacity = 5000);
void FlushRenderer (void);
//!@}

//!@{ Set values impacting the global debug renderer state
void SetLineWidth (float width);
void SetProjection (const math::mat4& projection);
//!@}

//!@{ Draw line
void DrawLine (const math::vec2& pa, const math::vec2& pb, const color& c = DEFAULT_COLOR);
void DrawLine (const math::vec3& pa, const math::vec3& pb, const color& c = DEFAULT_COLOR);
//!@}

//!@{ Draw wireframe
void DrawWireFrame (const SpriteVertices& vertices, const color& c = DEFAULT_COLOR);
//!@}

//!@{ Draw wireframe for physics objects (ratio is the pixels per meter)
void DrawWireFrame (const physics::aabb& box,
                    const color& c = DEFAULT_COLOR,
                    float ratio = 1.f);
void DrawWireFrame (const physics::circle& circle,
                    const color& c = DEFAULT_COLOR,
                    float ratio = 1.f);
void DrawWireFrame (const physics::polygon& poly,
                    const color& c = DEFAULT_COLOR,
                    float ratio = 1.f);
void DrawWireFrame (const physics::Fixture* fixture,
                    const color& c = DEFAULT_COLOR,
                    float ratio = 1.f);
//!@}

#else
inline void InitializeRenderer (void) { }
inline void InitializeRenderer (uint16) { }
inline void FlushRenderer (void) { }
inline void SetLineWidth (float) { }
inline void SetProjection (const math::mat4&) { }
inline void DrawLine (const math::vec3&, const math::vec3&, const color&) { }
template <typename T> inline void DrawWireFrame (const T&, const color&) { }
template <typename T> inline void DrawWireFrame (const T&, float, const color&) { }
#endif

} // namespace debug
} // namespace rdge
