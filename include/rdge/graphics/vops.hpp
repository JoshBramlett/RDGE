//! \headerfile <rdge/graphics/vops.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/23/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
//! \namespace vops Vertex operations
namespace vops {

// TODO change after vec3 is declared constexpr:  constexpr math::vec3
inline math::vec3
GetPosition (const SpriteVertices& vertices)
{
    return vertices[0].pos;
}

constexpr math::vec2
GetSize (const SpriteVertices& vertices)
{
    return { (vertices[2].pos.x - vertices[0].pos.x),
             (vertices[2].pos.y - vertices[0].pos.y) };
}

inline void
SetPosition (SpriteVertices& vertices, const math::vec3& pos, const math::vec2& size)
{
    vertices[0].pos = pos;
    vertices[1].pos = { pos.x, pos.y + size.y, pos.z };
    vertices[2].pos = { pos.x + size.x, pos.y + size.y, pos.z };
    vertices[3].pos = { pos.x + size.x, pos.y, pos.z };
}

inline void
SetTexCoords (SpriteVertices& vertices, const tex_coords& uvs = tex_coords())
{
    vertices[0].uv = uvs[0];
    vertices[1].uv = uvs[1];
    vertices[2].uv = uvs[2];
    vertices[3].uv = uvs[3];
}

inline void
SetTextureUnitID (SpriteVertices& vertices, uint32 unit_id)
{
    vertices[0].tid = unit_id;
    vertices[1].tid = unit_id;
    vertices[2].tid = unit_id;
    vertices[3].tid = unit_id;
}

inline void
SetColor (SpriteVertices& vertices, const color& color)
{
    auto packed_color = static_cast<uint32>(color);
    vertices[0].color = packed_color;
    vertices[1].color = packed_color;
    vertices[2].color = packed_color;
    vertices[3].color = packed_color;
}

inline void
UpdatePosition (SpriteVertices& vertices, const math::vec3& pos)
{
    auto size = GetSize(vertices);
    vertices[1].pos = pos;
    vertices[2].pos = { pos.x, pos.y + size.y, pos.z };
    vertices[2].pos = { pos.x + size.x, pos.y + size.y, pos.z };
    vertices[3].pos = { pos.x + size.x, pos.y, pos.z };
}

inline void
UpdateDepth (SpriteVertices& vertices, float32 depth)
{
    vertices[0].pos.z = depth;
    vertices[1].pos.z = depth;
    vertices[2].pos.z = depth;
    vertices[3].pos.z = depth;
}

inline void
UpdateSize (SpriteVertices& vertices, const math::vec2& size)
{
    auto x2 = vertices[0].pos.x + size.x;
    auto y2 = vertices[0].pos.y + size.y;

    vertices[1].pos.y = y2;
    vertices[2].pos.x = x2;
    vertices[2].pos.y = y2;
    vertices[3].pos.x = x2;
}

} // namespace vops
} // namespace rdge
