#include <rdge/gameobjects/text.hpp>
#include <rdge/texture.hpp>

#include <cmath>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

Text::Text (
            std::string                 text,
            std::shared_ptr<RDGE::Assets::Font> font,
            RDGE::Graphics::Point       destination,
            RDGE::Color                 color,
            RDGE::Assets::Font::RenderMode      mode,
            TextAlignment               align
           )
    : m_text(std::move(text))
    , m_font(std::move(font))
    , m_destination(std::move(destination))
    , m_color(std::move(color))
    , m_renderMode(mode)
    , m_align(align)
    , m_textureValid(false)
    , m_cacheSurface(m_font->RenderUTF8(m_text, m_color, m_renderMode))
    , m_cacheTexture(nullptr)
{
    // IMPORTANT - m_cacheTexture is initialized with a nullptr.  This is perfectly
    //             fine, but we must ensure nothing is done with the object until it's
    //             properly initialized.

    CalculateDestination();
}

Text::Text (Text&& rhs) noexcept
    : m_text(std::move(rhs.m_text))
    , m_font(std::move(rhs.m_font))
    , m_destination(std::move(rhs.m_destination))
    , m_color(std::move(rhs.m_color))
    , m_cacheSurface(std::move(rhs.m_cacheSurface))
    , m_cacheTexture(std::move(rhs.m_cacheTexture))
{ }

Text&
Text::operator= (Text&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_text = std::move(rhs.m_text);
        m_font = std::move(rhs.m_font);
        m_destination = std::move(rhs.m_destination);
        m_color = std::move(rhs.m_color);
        m_cacheSurface = std::move(rhs.m_cacheSurface);
        m_cacheTexture = std::move(rhs.m_cacheTexture);
    }

    return *this;
}

void
Text::Render (const RDGE::Window& window)
{
    if (m_textureValid == false)
    {
        m_cacheTexture = RDGE::Texture(window.Renderer(), m_cacheSurface.RawPtr());
        SDL_SetTextureBlendMode(m_cacheTexture.RawPtr(), SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(m_cacheTexture.RawPtr(), m_color.a);

        m_textureValid = true;
    }

    window.Draw(m_cacheTexture, m_finalDestination);
}

void
Text::SetText (std::string text)
{
    m_text = std::move(text);
    CalculateDestination();

    m_cacheSurface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_textureValid = false;
}

void
Text::SetPosition (RDGE::Graphics::Point destination) noexcept
{
    m_destination = std::move(destination);
    CalculateDestination();
}

void
Text::SetColor (RDGE::Color color)
{
    // keep current opacity
    auto alpha = color.a;
    m_color = std::move(color);
    m_color.a = alpha;

    m_cacheSurface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_textureValid = false;
}

void
Text::SetOpacity (double opacity) noexcept
{
    // make sure opacity is in range [0,1]
    opacity = std::fmin(opacity, 1.0);
    opacity = std::fmax(opacity, 0.0);

    auto alpha = static_cast<RDGE::UInt8>(opacity * 255);
    m_color.a = alpha;

    if (m_textureValid)
    {
        SDL_SetTextureAlphaMod(m_cacheTexture.RawPtr(), m_color.a);
    }
}

void
Text::CalculateDestination (void)
{
    m_finalDestination = m_destination;

    // Could be called multiple times per frame.  Exit early if the
    // alignment is the default
    if (m_align == TextAlignment::TopLeft)
    {
        return;
    }

    auto size = m_font->SampleSizeUTF8(m_text);

    // adjust x coordinate to align center
    if (
        m_align == TextAlignment::TopCenter ||
        m_align == TextAlignment::MiddleCenter ||
        m_align == TextAlignment::BottomCenter
       )
    {
        m_finalDestination.x -= static_cast<RDGE::Int32>(size.w / 2);
    }

    // adjust x coordinate to align right
    if (
        m_align == TextAlignment::TopRight ||
        m_align == TextAlignment::MiddleRight ||
        m_align == TextAlignment::BottomRight
       )
    {
        m_finalDestination.x -= static_cast<RDGE::Int32>(size.w);
    }

    // adjust y coordinate to align center
    if (
        m_align == TextAlignment::MiddleLeft ||
        m_align == TextAlignment::MiddleCenter ||
        m_align == TextAlignment::MiddleRight
       )
    {
        m_finalDestination.y -= static_cast<RDGE::Int32>(size.h / 2);
    }

    // adjust x coordinate to align right
    if (
        m_align == TextAlignment::BottomLeft ||
        m_align == TextAlignment::BottomCenter ||
        m_align == TextAlignment::BottomRight
       )
    {
        m_finalDestination.y -= static_cast<RDGE::Int32>(size.h);
    }
}

} // namespace GameObjects
} // namespace RDGE
