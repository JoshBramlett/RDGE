#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/vops.hpp>

#include <SDL_assert.h>

namespace rdge {

Sprite::Sprite (const math::vec3& pos, const math::vec2& size, const color& color)
{
    vops::SetPosition(this->vertices, pos, size);
    vops::SetTexCoords(this->vertices);
    vops::SetColor(this->vertices, color);
}

Sprite::Sprite (const math::vec3& pos, std::shared_ptr<Texture> texture)
    : m_texture(texture)
{
    SDL_assert(m_texture != nullptr);

    math::vec2 size(static_cast<float>(texture->width), static_cast<float>(texture->height));
    vops::SetPosition(this->vertices, pos, size);
    vops::SetTexCoords(this->vertices);
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

    renderer.Submit(this->vertices);
}

void
Sprite::SetDepth (float depth)
{
    vops::UpdateDepth(this->vertices, depth);
}

} // namespace rdge
