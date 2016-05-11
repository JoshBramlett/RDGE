//! \headerfile <rdge/assets/font.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/29/2016
//! \todo Add support for Unicode

/* TODO: A lot of rendering functionality needs to be built in (there's like 10
 *       different methods to render text).  There's a bunch of getters and setters
 *       that need to be implemented as well.
 */

#pragma once

#include <rdge/types.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/surface.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Assets {

//! \typedef SDLFontUniquePtr
//! \details Proper unique_ptr type implementing SDL custom deleter
//! \note This wraps the native TTF_Font, not the RDGE::Font.  It's
//!       available as an alternative to the RAII object
using SDLFontUniquePtr = std::unique_ptr<TTF_Font, decltype(TTF_CloseFont)*>;

//! \brief Create unique_ptr wrapper for native TTF_Font
//! \details Helper function to wrap an TTF_Font in a unique_ptr
//!          with the proper custom deleter
//! \param [in] font Pre-allocated TTF_Font
//! \returns unique_ptr of provided TTF_Font
inline auto CreateSDLFontUniquePtr (TTF_Font* font) -> SDLFontUniquePtr
{
    return SDLFontUniquePtr(font, TTF_CloseFont);
}

//! \class Font
//! \brief RAII compliant wrapper for SDL TTF_Font
//! \details Provides functionality to load a font from file and fully
//!          configure how it'll be displayed when rendered.  Rendering of
//!          text will create a \ref RDGE::Surface object
//! \note SDL_ttf documentation states a seg fault will occur if a null
//!       TTF_Font is provided to many of it's functions.  The wrapper
//!       instead will perform strict null checks and throw an exception.
//! \warning UTF8 is currently the only supported text encoding
class Font : public std::enable_shared_from_this<Font>
{
public:
    //! \enum Style
    //! \brief Font style
    //! \details Direct mapping to SDL TTF style flasgs, provided for
    //!          ease of discovery
    enum class Style : RDGE::UInt32
    {
        Normal = TTF_STYLE_NORMAL,
        Bold = TTF_STYLE_BOLD,
        Italic = TTF_STYLE_ITALIC,
        Underline = TTF_STYLE_UNDERLINE,
        Strikethrough = TTF_STYLE_STRIKETHROUGH
    };

    //! \enum RenderMode
    //! \brief SDL_ttf supported modes for text rendering
    //! \details SDL_ttf provides the following rendering modes:
    //!            - [Solid] Fast, low quality.  Use when quickly updating
    //!            - [Shaded] Slower, better quality.  Background color is
    //!                         visible creating a box around the text.
    //!            - [Blended] Very slow, high quality.  Use for static text.
    //! \see https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html#SEC42
    enum class RenderMode : RDGE::UInt8
    {
        //! \var Solid
        //! \brief Fast, low quality rendering
        Solid,
        //! \var Shaded
        //! \brief Slow, and renders anti-aliased text with a background color
        Shaded,
        //! \var Blended
        //! \brief Slow, high quality anti-aliased 32-bit ARGB surface
        Blended
    };

    //! \brief Font ctor
    //! \details Wrapper for a font object
    //! \param [in] font Pointer to TTF_Font
    explicit Font (TTF_Font* font);

    //! \brief Font ctor
    //! \details Wrapper for a font object
    //! \param [in] file File path of the font to load
    //! \param [in] point_size Point size (based on 72 DPI) to load
    //! \param [in] index Font face index
    //! \throws Font initialization failed or SDL_ttf is not initialized
    explicit Font (
                   const std::string& file,
                   RDGE::UInt32       point_size,
                   RDGE::Int64        index = 0
                  );

    //! \brief Font dtor
    ~Font (void);

    //! \brief Font Copy ctor
    //! \details Non-copyable
    Font (const Font&) = delete;

    //! \brief Font Move ctor
    //! \details Transfers ownership of pointer
    Font (Font&& rhs) noexcept;

    //! \brief Font Copy Assignment Operator
    //! \details Non-copyable
    Font& operator= (const Font&) = delete;

    //! \brief Font Move Assignment Operator
    //! \details Transfers ownership of pointer
    Font& operator= (Font&& rhs) noexcept;

    //! \brief Safely retrieve additional shared_ptr instance
    //! \returns Shared pointer of the current object
    //! \throws std::bad_weak_ptr If called when object is not managed
    //!         by a shared_ptr
    std::shared_ptr<Font> GetSharedPtr (void) noexcept
    {
        return shared_from_this();
    }

    //! \brief Return the TTF_Font pointer
    //! \details Raw pointer is returned so caller must ensure
    //!          Font object will not fall out of scope
    //! \returns Raw pointer to a TTF_Font
    TTF_Font* RawPtr (void) const { return m_font; }

    //! \brief Bitmask of all set styles
    //! \return Native unsigned int
    //! \throws Font member is null
    RDGE::UInt32 Styles (void) const;

    //! \brief Checks if style has been set
    //! \param [in] style Style to check for
    //! \returns True if style is set, false otherwise
    //! \throws Font member is null
    bool HasStyle (enum Style style) const;

    //! \brief Add style to current set styles
    //! \param [in] style Style to set
    //! \throws Font member is null
    void AddStyle (enum Style style);

    //! \brief Reset styles to provided bitmask
    //! \param [in] style_flags Bitmask of styles
    //! \throws Font member is null
    void SetStyles (RDGE::UInt32 style_flags);

    //! \brief Check if the font is monospaced
    //! \details With a monospaced font you can assume the total surface
    //!          width by multiplying glyph width by string size
    //! \returns True if the font is monospaced, false if not
    //! \throws Font member is null
    bool IsMonospaced (void) const;

    //! \brief Get the surface size of the provided text after rendering
    //! \details No rendering is performed, this is provided to query
    //!          the size prior to rendering
    //! \param [in] text UTF8 text to be rendered
    //! \returns Size of surface
    //! \throws Font member is null
    RDGE::Graphics::Size SampleSizeUTF8 (const std::string& text);

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
    std::shared_ptr<Surface> RenderUTF8 (
                                         const std::string& text,
                                         const RDGE::Color& color,
                                         RenderMode         mode       = RenderMode::Solid,
                                         const RDGE::Color& background = RDGE::Color::Black()
                                        );

private:
    TTF_Font*   m_font;
};

inline RDGE::UInt32 operator| (Font::Style a, Font::Style b)
{
    return static_cast<RDGE::UInt32>(a) | static_cast<RDGE::UInt32>(b);
}

inline RDGE::UInt32 operator& (Font::Style a, Font::Style b)
{
    return static_cast<RDGE::UInt32>(a) & static_cast<RDGE::UInt32>(b);
}

inline RDGE::UInt32 operator| (RDGE::UInt32 a, Font::Style b)
{
    return a | static_cast<RDGE::UInt32>(b);
}

inline RDGE::UInt32 operator& (RDGE::UInt32 a, Font::Style b)
{
    return a & static_cast<RDGE::UInt32>(b);
}

} // namespace Assets
} // namespace RDGE
