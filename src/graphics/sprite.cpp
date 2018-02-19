#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/vops.hpp>

#include <rdge/debug/renderer.hpp>

#include <SDL_assert.h>

namespace rdge {

Sprite::Sprite (const math::vec3& pos, const math::vec2& size, const color& color)
{
    vops::SetPosition(this->vertices, pos, size);
    vops::SetDefaultTexCoords(this->vertices);
    vops::SetColor(this->vertices, color);
}

Sprite::Sprite (const math::vec3& pos, std::shared_ptr<Texture> texture)
    : m_texture(texture)
{
    SDL_assert(m_texture != nullptr);

    math::vec2 size(static_cast<float>(texture->Width()), static_cast<float>(texture->Height()));
    vops::SetPosition(this->vertices, pos, size);
    vops::SetDefaultTexCoords(this->vertices);
    vops::SetTextureUnitID(this->vertices, m_texture->unit_id);
}

Sprite::Sprite (const math::vec3&        pos,
                const math::vec2&        size,
                std::shared_ptr<Texture> texture,
                const tex_coords&        coords)
    : m_texture(texture)
{
    SDL_assert(m_texture != nullptr);

    vops::SetPosition(this->vertices, pos, size);
    vops::SetTexCoords(this->vertices, coords);
    vops::SetTextureUnitID(this->vertices, m_texture->unit_id);
}

void
Sprite::SetRenderTarget (SpriteBatch& renderer)
{
    if (!m_texture)
    {
        return;
    }

    renderer.RegisterTexture(m_texture);
    vops::SetTextureUnitID(this->vertices, m_texture->unit_id);
}

void
Sprite::Draw (SpriteBatch& renderer)
{
    // The texture must be registered with the renderer prior to constructing
    // the sprite.  If not the unit_id will not be updated with the vertex
    // data, so when we draw it'll use the invalid ID.  The following assert
    // makes sure the texture ids match.
    SDL_assert(!m_texture || this->vertices[0].tid == m_texture->unit_id);

    if (visible)
    {
        renderer.Submit(this->vertices);

#if RDGE_DEBUG
    if (this->debug_bounds.show)
    {
        debug::DrawWireFrame(this->vertices, this->debug_bounds.draw_color);
    }
#endif
    }
}

void
Sprite::SetDepth (float depth)
{
    vops::SetDepth(this->vertices, depth);
}

} // namespace rdge
