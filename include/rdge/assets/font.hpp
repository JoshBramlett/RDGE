//! \headerfile <rdge/assets/font.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

/* TODO: A lot of rendering functionality needs to be built in (there's like 10
 *       different methods to render text).  There's a bunch of getters and setters
 *       that need to be implemented as well.
 *
 *       Support for unicode (and wchar_t) needs to be added
 */

#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/color.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \typedef SDLFontUniquePtr
//! \details Encosing unique_ptr type that includes the SDL custom deleter
using SDLFontUniquePtr = std::unique_ptr<TTF_Font, decltype(TTF_CloseFont)*>;

//! \brief Create SDL TTF_Font unique_ptr with a proper deleter
//! \param [in] font Pre-allocated TTF_Font
//! \returns unique_ptr of provided TTF_Font
inline auto CreateManagedSDLFont (TTF_Font* font) -> SDLFontUniquePtr
{
    return SDLFontUniquePtr(font, TTF_CloseFont);
}

//! \class Font
//! \brief Wrapper for an SDL TTF_Font
//! \details Provides functionality to load a font from disk and fully
//!          configure how it'll be displayed when rendered.  Rendering of
//!          text will create a \ref rdge::Surface object.
//! \note UTF8 is currently the only supported text encoding
//! \warning SDL_ttf documentation states a seg fault will occur if a null
//!          TTF_Font is provided to many of it's functions.  The wrapper
//!          instead will perform strict null checks and throw an exception.
//! \see rdge::Surface
class Font
{
public:
    //! \enum Style
    //! \brief Font style bitmask
    //! \details Direct mapping to SDL TTF style flasgs, provided for
    //!          ease of discovery
    //! \note is_enum_bitmask is enabled and supports bitwise operations
    enum class Style : uint32
    {
        NORMAL        = TTF_STYLE_NORMAL,
        BOLD          = TTF_STYLE_BOLD,
        ITALIC        = TTF_STYLE_ITALIC,
        UNDERLINE     = TTF_STYLE_UNDERLINE,
        STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH
    };

    //! \enum RenderMode
    //! \brief SDL_ttf supported modes for text rendering
    //! \details SDL_ttf provides the following rendering modes:
    //!            - [Solid] Fast, low quality.  Use when quickly updating
    //!            - [Shaded] Slower, better quality.  Background color is
    //!                         visible creating a box around the text.
    //!            - [Blended] Very slow, high quality.  Use for static text.
    //! \see https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html#SEC42
    enum class RenderMode : uint8
    {
        SOLID,  //!< Fast, low quality rendering
        SHADED, //!< Slow, and renders anti-aliased text with a background color
        BLENDED //!< Slow, high quality anti-aliased 32-bit ARGB surface
    };

    //! \brief Font ctor
    //! \details Creates a font from a native SDL TTF_Font
    //! \param [in] font Pointer to TTF_Font
    explicit Font (TTF_Font* font);

    //! \brief Font ctor
    //! \details Wrapper for a font object
    //! \param [in] path File path of the font to load
    //! \param [in] point_size Point size (based on 72 DPI) to load
    //! \param [in] index Font face index if the file contains multiple faces.
    //!                   The first face is always at index 0
    //! \throws rdge::SDLException Font initialization failed
    explicit Font (const std::string& path, uint32 point_size, int64 index = 0);

    //! \brief Font dtor
    ~Font (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    Font (const Font&) = delete;
    Font& operator= (const Font&) = delete;
    Font (Font&&) noexcept;
    Font& operator= (Font&&) noexcept;
    //!@}

    //! \brief User defined conversion to a raw TTF_Font pointer
    //! \warning Be careful not to dereference the pointer after the
    //!          parent Font object falls out of scope
    //! \returns Raw pointer to a TTF_Font
    explicit operator const TTF_Font* (void) const noexcept
    {
        return m_font;
    }

    //! \brief All enabled font styles
    //! \return Style bitmask
    //! \throws rdge::Exception Font member is null
    Style GetStyles (void) const;

    //! \brief Checks if style(s) is/are enabled
    //! \details If a bitmask is supplied, all values must be set.
    //! \param [in] flags Style(s) to check for
    //! \returns True if set, false otherwise
    //! \throws rdge::Exception Font member is null
    bool HasStyle (Style flags) const;

    //! \brief Add style to the currently enabled styles
    //! \details If a bitmask is supplied, all values will be set.
    //! \param [in] flags Style(s) to add
    //! \throws rdge::Exception Font member is null
    void AddStyle (Style flags);

    //! \brief Reset styles to the provided value
    //! \details If a bitmask is supplied, all values will be set.
    //! \param [in] flags Style(s) to set
    //! \throws rdge::Exception Font member is null
    void SetStyle (Style flags);

    //! \brief Check if the font is monospaced
    //! \details With a monospaced font you can assume the total surface
    //!          width by multiplying glyph width by string size
    //! \returns True if the font is monospaced, false if not
    //! \throws rdge::Exception Font member is null
    bool IsMonospaced (void) const;

    //! \brief Get the surface size of the provided text after rendering
    //! \details No rendering is performed, this is provided to query
    //!          the size prior to rendering.
    //! \param [in] text UTF8 text to be rendered
    //! \returns Size of surface
    //! \throws rdge::Exception Font member is null
    size SampleSizeUTF8 (const std::string& text);

    //! \brief Render UTF8 text to a Surface
    //! \details Rendering defaults to Solid mode, which is the fastest
    //!          but offers lower quality.  Specifying any other mode
    //!          should only be done on text updated infrequently
    //! \param [in] text Text to render
    //! \param [in] color Color to render the text
    //! \param [in] mode SDL_ttf render mode
    //! \param [in] background Background color (for Shaded mode only)
    //! \returns Surface object of the rendered font
    //! \throws Text rendering failed
    std::shared_ptr<Surface>
    RenderUTF8 (const std::string& text,
                const rdge::color& color,
                RenderMode         mode       = RenderMode::SOLID,
                const rdge::color& background = color::BLACK);

private:
    TTF_Font* m_font = nullptr; //!< SDL Font object
};

//! \brief Enable Font::Style enum for bitmask operations
template <>
struct is_enum_bitmask<Font::Style> : public std::true_type { };

} // namespace rdge
