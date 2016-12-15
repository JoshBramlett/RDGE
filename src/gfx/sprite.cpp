#include <rdge/gfx/sprite.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

using namespace rdge;
using namespace rdge::gfx;
using namespace rdge::math;

Sprite::Sprite (float x, float y, float width, float height, const color& color)
    : Renderable2D(vec3(x, y, 0), vec2(width, height))
{
    m_color = color;
    m_uv = DefaultUVCoordinates();
}

Sprite::Sprite (
                float x, float y,
                float width, float height,
                std::shared_ptr<rdge::Texture> texture,
                UVCoordinates uv
               )
    : Renderable2D(vec3(x, y, 0), vec2(width, height))
{
    m_texture = std::move(texture);
    m_uv = uv;
}
