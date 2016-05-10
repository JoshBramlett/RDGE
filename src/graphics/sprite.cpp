#include <rdge/graphics/sprite.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

namespace RDGE {
namespace Graphics {

using namespace RDGE::Math;

Sprite::Sprite (float x, float y, float width, float height, const RDGE::Color& color)
    : Renderable2D(vec3(x, y, 0), vec2(width, height))
{
    m_color = color;
}

Sprite::Sprite (
                float x, float y,
                float width, float height,
                std::shared_ptr<GLTexture> texture,
                UVCoordinates uv
               )
    : Renderable2D(vec3(x, y, 0), vec2(width, height))
{
    m_texture = std::move(texture);
    m_uv = uv;
}

} // namespace Graphics
} // namespace RDGE
