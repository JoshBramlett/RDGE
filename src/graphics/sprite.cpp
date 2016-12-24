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

Sprite::Sprite (const math::vec3&        pos,
                const math::vec2&        size,
                std::shared_ptr<Texture> texture,
                const tex_coords&        coords)
    : m_texture(texture)
{
    vops::SetPosition(this->vertices, pos, size);
    vops::SetTexCoords(this->vertices, coords);
}

Sprite::Sprite (Sprite&& rhs) noexcept
    : m_texture(std::move(rhs.m_texture))
{
    std::swap(this->vertices, rhs.vertices);
}

Sprite&
Sprite::operator= (Sprite&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_texture = std::move(rhs.m_texture);
        std::swap(this->vertices, rhs.vertices);
    }

    return *this;
}

void
Sprite::Draw (SpriteBatch& renderer)
{
    renderer.Submit(this->vertices);
}

} // namespace rdge
