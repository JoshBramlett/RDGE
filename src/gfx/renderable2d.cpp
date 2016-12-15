#include <rdge/gfx/renderable2d.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/internal/exception_macros.hpp>

using namespace rdge;
using namespace rdge::gfx;
using namespace rdge::math;

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
// TODO should be reference
Renderable2D::Submit (Renderer2D* renderer) const
{
    renderer->Submit(this);
}

void
Renderable2D::SetPosition (const rdge::math::vec3& position)
{
    m_position = position;
}

void
Renderable2D::SetSize (const rdge::math::vec2& size)
{
    m_size = size;
}

void
Renderable2D::SetColor (const rdge::gfx::color& color, bool ignore_alpha)
{
    if (ignore_alpha)
    {
        m_color.r = color.r;
        m_color.g = color.g;
        m_color.b = color.b;
    }
    else
    {
        m_color = color;
    }
}

void
Renderable2D::SetUV (const UVCoordinates& uv)
{
    m_uv = uv;
}

void
Renderable2D::SetOpacity (rdge::uint8 opacity)
{
    m_color.a = opacity;
}

void
Renderable2D::SetOpacity (float opacity)
{
    auto clamped = clamp(opacity, 0.0f, 1.0f);

    m_color.a = static_cast<rdge::uint8>(clamped * 255);
}

void
Renderable2D::SetZIndex (float zindex)
{
    m_position.z = clamp(zindex, 0.0f, 1.0f);
}

void
Renderable2D::Scale (float scaler)
{
    if (scaler <= 0.0f)
    {
        RDGE_THROW("Scaler must be a positive value");
    }

    m_size *= scaler;
}
