#include <rdge/graphics/renderable2d.hpp>
#include <rdge/internal/exception_macros.hpp>

namespace RDGE {
namespace Graphics {

Renderable2D::Renderable2D (Renderable2D&& rhs) noexcept
    : m_position(rhs.m_position)
    , m_size(rhs.m_size)
    , m_color(rhs.m_color)
    , m_texture(std::move(rhs.m_texture))
    , m_uv(std::move(rhs.m_uv))
{ }

Renderable2D&
Renderable2D::operator= (Renderable2D&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_position = rhs.m_position;
        m_size = rhs.m_size;
        m_color = rhs.m_color;

        m_texture = std::move(rhs.m_texture);
        m_uv = std::move(rhs.m_uv);
    }

    return *this;
}

void
Renderable2D::Submit (Renderer2D* renderer) const
{
    renderer->Submit(this);
}

void
Renderable2D::SetPosition (const RDGE::Math::vec3& position)
{
    m_position = position;
}

void
Renderable2D::SetSize (const RDGE::Math::vec2& size)
{
    m_size = size;
}

void
Renderable2D::SetColor (const RDGE::Color& color, bool ignore_alpha)
{
    if (ignore_alpha)
    {
        m_color = color;
    }
    else
    {
        m_color.r = color.r;
        m_color.g = color.g;
        m_color.b = color.b;
    }
}

void
Renderable2D::SetOpacity (RDGE::UInt8 opacity)
{
    m_color.a = opacity;
}

void
Renderable2D::SetZIndex (float zindex)
{
    m_position.z = RDGE::Math::clamp(zindex, 0.0f, 1.0f);
}

void
Renderable2D::Scale (float scaler)
{
    if (scaler <= 0)
    {
        RDGE_THROW("Scaler must be a positive value");
    }

    m_size *= scaler;
}

} // namespace Graphics
} // namespace RDGE
