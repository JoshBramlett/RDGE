#include <rdge/font.hpp>
#include <rdge/internal/exception_macros.hpp>

namespace RDGE {

Font::Font (TTF_Font* font)
    : m_font(font)
{ }

Font::Font (
            const std::string& file,
            RDGE::UInt32       point_size,
            RDGE::Int64        index
           )
    : m_font(nullptr)
{
    if (TTF_WasInit() == 0)
    {
        RDGE_THROW("SDL_ttf has not yet been initialized");
    }

    m_font = TTF_OpenFontIndex(file.c_str(), point_size, index);
    if (UNLIKELY(!m_font))
    {
        SDL_THROW("Failed to load font. file=" + file, "TTF_OpenFontIndex");
    }
}

Font::~Font (void)
{
    if (m_font != nullptr)
    {
        TTF_CloseFont(m_font);
    }
}

Font::Font (Font&& rhs) noexcept
    : m_font(rhs.m_font)
{
    rhs.m_font = nullptr;
}

Font&
Font::operator= (Font&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_font != nullptr)
        {
            TTF_CloseFont(m_font);
        }

        m_font = rhs.m_font;
        rhs.m_font = nullptr;
    }

    return *this;
}

RDGE::UInt32
Font::Styles (void) const
{
    if (UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to get the style of a null font object");
    }

    return TTF_GetFontStyle(m_font);
}

bool
Font::HasStyle (enum Style style) const
{
    // Styles() will perform the null check for us
    RDGE::UInt32 styles = Styles();

    return styles & static_cast<RDGE::UInt32>(style);
}

void
Font::AddStyle (enum Style style)
{
    // Styles() will perform the null check for us
    RDGE::UInt32 styles = Styles();
    styles |= static_cast<RDGE::UInt32>(style);

    SetStyles(styles);
}

void
Font::SetStyles (RDGE::UInt32 style_flags)
{
    if (UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to set the style of a null font object");
    }

    TTF_SetFontStyle(m_font, style_flags);
}

bool
Font::IsMonospaced (void) const
{
    if (UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to set the style of a null font object");
    }

    return TTF_FontFaceIsFixedWidth(m_font);
}

RDGE::Graphics::Size
Font::SampleSizeUTF8 (const std::string& text)
{
    if (UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to sample the size using a null font object");
    }

    RDGE::Int32 w, h;
    if (UNLIKELY(TTF_SizeUTF8(m_font, text.c_str(), &w, &h) != 0))
    {
        SDL_THROW("Failed to sample surface size", "TTF_SizeUTF8");
    }

    return RDGE::Graphics::Size(w, h);
}

RDGE::Surface
Font::RenderUTF8 (
                  const std::string& text,
                  const RDGE::Color& color,
                  RenderMode         mode,
                  const RDGE::Color& background
                 )
{
    if (UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to render text using a null font object");
    }

    SDL_Surface* surface = nullptr;
    if (mode == RenderMode::Solid)
    {
        surface = TTF_RenderUTF8_Solid(m_font, text.c_str(), color);
    }
    else if (mode == RenderMode::Shaded)
    {
        surface = TTF_RenderUTF8_Shaded(m_font, text.c_str(), color, background);
    }
    else if (mode == RenderMode::Blended)
    {
        surface = TTF_RenderUTF8_Blended(m_font, text.c_str(), color);
    }

    if (UNLIKELY(!surface))
    {
        // When other encodings are supported this should be re-implemented
        std::string fn_name = "TTF_RenderUTF8_";
        switch (mode)
        {
        case RenderMode::Solid:
            fn_name += "Solid";
            break;
        case RenderMode::Shaded:
            fn_name += "Shaded";
            break;
        case RenderMode::Blended:
            fn_name += "Blended";
            break;
        }

        SDL_THROW("Render text failed", fn_name);
    }

    return RDGE::Surface { surface };
}

} // namespace RDGE
