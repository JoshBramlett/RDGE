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

constexpr math::vec3
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
SetPosition (SpriteVertices& vertices, const math::vec3& pos)
{
    SetPosition(vertices, pos, GetSize(vertices));
}

inline void
SetPosition (SpriteVertices& vertices, const math::vec2& pos)
{
    const auto size = GetSize(vertices);
    vertices[0].pos.x = pos.x;
    vertices[0].pos.y = pos.y;
    vertices[1].pos.x = pos.x;
    vertices[1].pos.y = pos.y + size.y;
    vertices[2].pos.x = pos.x + size.x;
    vertices[2].pos.y = pos.y + size.y;
    vertices[3].pos.x = pos.x + size.x;
    vertices[3].pos.y = pos.y;
}

inline void
ClonePosition (const SpriteVertices& source, SpriteVertices& dest)
{
    dest[0].pos = source[0].pos;
    dest[1].pos = source[1].pos;
    dest[2].pos = source[2].pos;
    dest[3].pos = source[3].pos;
}

inline void
SetTexCoords (SpriteVertices& vertices, const tex_coords& uvs)
{
    vertices[0].uv = uvs[0];
    vertices[1].uv = uvs[1];
    vertices[2].uv = uvs[2];
    vertices[3].uv = uvs[3];
}

inline void
SetDefaultTexCoords (SpriteVertices& vertices)
{
    SetTexCoords(vertices, tex_coords());
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
UpdatePosition (SpriteVertices& vertices, const math::vec2& pos)
{
    vertices[0].pos += pos;
    vertices[1].pos += pos;
    vertices[2].pos += pos;
    vertices[3].pos += pos;
}

inline void
SetDepth (SpriteVertices& vertices, float32 depth)
{
    vertices[0].pos.z = depth;
    vertices[1].pos.z = depth;
    vertices[2].pos.z = depth;
    vertices[3].pos.z = depth;
}

inline void
SetSize (SpriteVertices& vertices, const math::vec2& size)
{
    auto x2 = vertices[0].pos.x + size.x;
    auto y2 = vertices[0].pos.y + size.y;

    vertices[1].pos.y = y2;
    vertices[2].pos.x = x2;
    vertices[2].pos.y = y2;
    vertices[3].pos.x = x2;
}

inline void
FlipHorizontal (SpriteVertices& vertices)
{
    std::swap(vertices[0].uv, vertices[3].uv);
    std::swap(vertices[1].uv, vertices[2].uv);
}

inline void
FlipVertical (SpriteVertices& vertices)
{
    std::swap(vertices[0].uv, vertices[1].uv);
    std::swap(vertices[2].uv, vertices[3].uv);
}

} // namespace vops
} // namespace rdge
