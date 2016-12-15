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
#include <rdge/gfx/color.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace assets {

//! \typedef SDLFontUniquePtr
//! \details Encosing unique_ptr type that includes the SDL custom deleter
using SDLFontUniquePtr = std::unique_ptr<TTF_Font, decltype(TTF_CloseFont)*>;

//! \brief Create SDL TTF_Font unique_ptr with a proper deleter
//! \param [in] font Pre-allocated TTF_Font
//! \returns unique_ptr of provided TTF_Font
inline auto CreateSDLFontUniquePtr (TTF_Font* font) -> SDLFontUniquePtr
{
    return SDLFontUniquePtr(font, TTF_CloseFont);
}

//! \class Font
//! \brief Wrapper for an SDL TTF_Font
//! \details Provides functionality to load a font from disk and fully
//!          configure how it'll be displayed when rendered.  Rendering of
//!          text will create a \ref rdge::assets::Surface object.
//! \note UTF8 is currently the only supported text encoding
//! \warning SDL_ttf documentation states a seg fault will occur if a null
//!          TTF_Font is provided to many of it's functions.  The wrapper
//!          instead will perform strict null checks and throw an exception.
//! \see rdge::Surface
class Font final : public std::enable_shared_from_this<Font>
{
public:
    //! \enum Style
    //! \brief Font style bitmask
    //! \details Direct mapping to SDL TTF style flasgs, provided for
    //!          ease of discovery
    //! \note is_enum_bitmask is enabled and supports bitwise operations
    enum class Style : rdge::uint32
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
    enum class RenderMode : rdge::uint8
    {
        //! \var SOLID
        //! \brief Fast, low quality rendering
        SOLID,
        //! \var SHADED
        //! \brief Slow, and renders anti-aliased text with a background color
        SHADED,
        //! \var BLENDED
        //! \brief Slow, high quality anti-aliased 32-bit ARGB surface
        BLENDED
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
    explicit Font (const std::string& path,
                   rdge::uint32       point_size,
                   rdge::int64        index = 0);

    //! \brief Font dtor
    ~Font (void) noexcept;

    //! \brief Font Copy ctor
    //! \details Non-copyable
    Font (const Font&) = delete;

    //! \brief Font Move ctor
    //! \details Transfers ownership
    Font (Font&&) noexcept;

    //! \brief Font Copy Assignment Operator
    //! \details Non-copyable
    Font& operator= (const Font&) = delete;

    //! \brief Font Move Assignment Operator
    //! \details Transfers ownership
    Font& operator= (Font&&) noexcept;

    //! \brief User defined conversion to a raw TTF_Font pointer
    //! \warning Be careful not to dereference the pointer after the
    //!          parent Font object falls out of scope
    //! \returns Raw pointer to a TTF_Font
    explicit operator const TTF_Font* (void) const noexcept
    {
        return m_font;
    }

    //! \brief Safely clone shared_ptr instance
    //! \returns Shared pointer of the current object
    //! \throws std::bad_weak_ptr If called when object is not managed
    //!         by a shared_ptr
    std::shared_ptr<Font> Clone (void)
    {
        return shared_from_this();
    }

    //! \brief All enabled font styles
    //! \return Style bitmask
    //! \throws rdge::Exception Font member is null
    Font::Style GetStyles (void) const;

    //! \brief Checks if style(s) is/are enabled
    //! \details If a bitmask is supplied, all values must be set.
    //! \param [in] flags Style(s) to check for
    //! \returns True if set, false otherwise
    //! \throws rdge::Exception Font member is null
    bool HasStyle (Font::Style flags) const;

    //! \brief Add style to the currently enabled styles
    //! \details If a bitmask is supplied, all values will be set.
    //! \param [in] flags Style(s) to add
    //! \throws rdge::Exception Font member is null
    void AddStyle (Font::Style flags);

    //! \brief Reset styles to the provided value
    //! \details If a bitmask is supplied, all values will be set.
    //! \param [in] flags Style(s) to set
    //! \throws rdge::Exception Font member is null
    void SetStyle (Font::Style flags);

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
    rdge::gfx::size SampleSizeUTF8 (const std::string& text);

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
    RenderUTF8 (const std::string&      text,
                const rdge::gfx::color& color,
                Font::RenderMode        mode       = Font::RenderMode::SOLID,
                const rdge::gfx::color& background = rdge::gfx::color::BLACK());

private:
    TTF_Font* m_font;
};

} // namespace assets

//! \brief Enable Font::Style enum for bitmask operations
template <>
struct is_enum_bitmask<assets::Font::Style> : public std::true_type { };

} // namespace rdge
