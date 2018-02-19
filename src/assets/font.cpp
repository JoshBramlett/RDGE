#include <rdge/assets/font.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/util/compiler.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <nothings/stb_truetype.h>

namespace rdge {

Font::Font (TTF_Font* font)
    : m_font(font)
{ }

Font::Font (const std::string& file, uint32 point_size, int64 index)
{
    if (TTF_WasInit() == 0)
    {
        // perform lazy loading if not already initialized through the application
        if (RDGE_UNLIKELY(TTF_Init() != 0))
        {
            SDL_THROW("SDL_ttf failed to initialize", "TTF_Init");
        }
    }

    m_font = TTF_OpenFontIndex(file.c_str(), point_size, index);
    if (RDGE_UNLIKELY(!m_font))
    {
        SDL_THROW("Failed to load font. file=" + file, "TTF_OpenFontIndex");
    }
}

Font::Font (const std::string& file)
{
    loaded_file lf = read_file(file.c_str());

    stbtt_fontinfo font;
    stbtt_InitFont(&font, (uint8*)lf.data, stbtt_GetFontOffsetForIndex((uint8*)lf.data, 0));

    int32 w, h;
    int32 scale = 20;
    uint8* bitmap = stbtt_GetCodepointBitmap(&font,
                                             0,
                                             stbtt_ScaleForPixelHeight(&font, scale),
                                             'a', &w, &h, 0, 0);
    STBTT_free(bitmap, nullptr);
}

Font::~Font (void) noexcept
{
    if (m_font)
    {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}

Font::Font (Font&& rhs) noexcept
{
    std::swap(m_font, rhs.m_font);
}

Font&
Font::operator= (Font&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_font, rhs.m_font);
    }

    return *this;
}

Font::Style
Font::GetStyles (void) const
{
    if (RDGE_UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to get the style of a null font object");
    }

    return static_cast<Font::Style>(TTF_GetFontStyle(m_font));
}

bool
Font::HasStyle (Font::Style flags) const
{
    auto styles = GetStyles();
    return static_cast<bool>(styles & flags);
}

void
Font::AddStyle (Font::Style flags)
{
    auto styles = GetStyles();
    SetStyle(styles | flags);
}

void
Font::SetStyle (Font::Style flags)
{
    if (RDGE_UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to set the style of a null font object");
    }

    TTF_SetFontStyle(m_font, static_cast<uint32>(flags));
}

bool
Font::IsMonospaced (void) const
{
    if (RDGE_UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to set the style of a null font object");
    }

    return TTF_FontFaceIsFixedWidth(m_font);
}

math::uivec2
Font::SampleSizeUTF8 (const std::string& text)
{
    if (RDGE_UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to sample the size using a null font object");
    }

    int32 w, h;
    if (RDGE_UNLIKELY(TTF_SizeUTF8(m_font, text.c_str(), &w, &h) != 0))
    {
        SDL_THROW("Failed to sample surface size", "TTF_SizeUTF8");
    }

    return { static_cast<uint32>(w), static_cast<uint32>(h) };
}

std::shared_ptr<Surface>
Font::RenderUTF8 (const std::string& text,
                  const rdge::color& color,
                  Font::RenderMode   mode,
                  const rdge::color& background)
{
    if (RDGE_UNLIKELY(!m_font))
    {
        RDGE_THROW("Trying to render text using a null font object");
    }

    SDL_Surface* surface = nullptr;
    switch (mode)
    {
    case RenderMode::SOLID:
        surface = TTF_RenderUTF8_Solid(m_font, text.c_str(), color);
        break;
    case RenderMode::SHADED:
        surface = TTF_RenderUTF8_Shaded(m_font, text.c_str(), color, background);
        break;
    case RenderMode::BLENDED:
        surface = TTF_RenderUTF8_Blended(m_font, text.c_str(), color);
        break;
    default:
        RDGE_THROW("Invalid RenderMode. mode=" + std::to_string(static_cast<int>(mode)));
    }

    if (RDGE_UNLIKELY(!surface))
    {
        SDL_THROW("Render text failed", "TTF_RenderUTF8_XXX");
    }

    return std::make_shared<Surface>(surface);
}

} // namespace rdge
