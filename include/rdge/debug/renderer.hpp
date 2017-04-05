//! \headerfile <rdge/debug/renderer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/02/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

#ifdef RDGE_DEBUG

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
void SetLineColor (const color& c);
void SetProjection (const math::mat4& projection);
//!@}

//!@{ Draw line (with color override)
void DrawLine (const math::vec3& pa, const math::vec3& pb);
void DrawLine (const math::vec3& pa, const math::vec3& pb, const color& c);
//!@}

//!@{ Draw wireframe (with color override)
void DrawWireFrame (const SpriteVertices& vertices);
void DrawWireFrame (const SpriteVertices& vertices, const color& c);
//!@}

#else
inline void InitializeRenderer (void) { }
inline void InitializeRenderer (uint16) { }
inline void FlushRenderer (void) { }
inline void SetLineWidth (float) { }
inline void SetLineColor (const color&) { }
inline void SetProjection (const math::mat4&) { }
inline void DrawLine (const math::vec3&, const math::vec3&) { }
inline void DrawLine (const math::vec3&, const math::vec3&, const color&) { }
inline void DrawWireFrame (const SpriteVertices&) { }
inline void DrawWireFrame (const SpriteVertices&, const color&) { }
#endif

} // namespace debug
} // namespace rdge
