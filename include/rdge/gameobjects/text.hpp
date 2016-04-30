//! \headerfile <rdge/gameobjects/text.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 02/15/2015
//! \bug

#pragma once

#include <string>
#include <memory>

#include <rdge/types.hpp>
#include <rdge/texture.hpp>
#include <rdge/window.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/gameobjects/ientity.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \enum TextAlignment
//! \brief Align the text relative to the destination
enum class TextAlignment : RDGE::UInt8
{
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

//! \class Text
//! \brief Helper for rendering text to the screen
//! \details Supported \ref RDGE::Assets::Font::RenderMode values are Solid
//!          (quick and low quality) or Blended (slow and high quality).
//!          Caching of surface and texture objects is done so as to
//!          allow the rendering code to be as lightweight as possible.
class Text : public IEntity
{
public:
    //! \brief Text ctor
    //! \param [in] text Text to render to the screen
    //! \param [in] font Shared pointer to font
    //! \param [in] point Desination coordinates
    //! \param [in] color Rendering color
    //! \param [in] mode Font render mode
    //! \param [in] align Text alignment relative to desination
    explicit Text (
                   std::string                 text,
                   std::shared_ptr<RDGE::Assets::Font> font,
                   RDGE::Graphics::Point       destination,
                   RDGE::Color                 color = RDGE::Color::White(),
                   RDGE::Assets::Font::RenderMode      mode  = RDGE::Assets::Font::RenderMode::Solid,
                   TextAlignment               align = TextAlignment::TopLeft
                  );

    //! \brief Text dtor
    virtual ~Text (void) { }

    //! \brief Text Copy ctor
    //! \details Non-copyable
    Text (const Text&) = delete;

    //! \brief Text Move ctor
    //! \details Transfers ownership of smart pointers
    Text (Text&&) noexcept;

    //! \brief Text Copy Assignment Operator
    //! \details Non-copyable
    Text& operator=(const Text&) = delete;

    //! \brief Text Move Assignment Operator
    //! \details Transfers ownership of smart pointers
    Text& operator=(Text&&) noexcept;

    //! \brief IEntity HandleEvents
    //! \details Intentionally empty
    virtual void HandleEvents (const SDL_Event&) { }

    //! \brief IEntity Update
    //! \details Intentionally empty
    virtual void Update (RDGE::UInt32) { }

    //! \brief IEntity Render
    //! \details Draws static text on game loop render phase
    virtual void Render (const RDGE::Window&);

    //! \brief IEntity Tag
    virtual std::string Tag (void) const
    {
        return "Text";
    }

    //! \brief Update the text for the next render call
    //! \param [in] text Text to render to the screen
    void SetText (std::string text);

    //! \brief Update the destination for the next render call
    //! \param [in] destination Desination coordinates
    void SetPosition (RDGE::Graphics::Point destination) noexcept;

    //! \brief Update the color of the text
    //! \param [in] color Color to set
    void SetColor (RDGE::Color color);

    //! \brief Update the drawing opacity
    //! \param [in] opacity Value in range [0, 1] representing a percentage
    void SetOpacity (double opacity) noexcept;

private:

    //! \brief Calculate and update the rendering destination
    //! \details Value should be recalculated whenever text or destination
    //!          has been updated
    void CalculateDestination (void);

    std::string                 m_text;
    std::shared_ptr<RDGE::Assets::Font> m_font;
    RDGE::Graphics::Point       m_destination;
    RDGE::Color                 m_color;
    RDGE::Assets::Font::RenderMode      m_renderMode;
    TextAlignment               m_align;

    // cache vars to avoid logic in game loop
    bool                  m_textureValid;
    RDGE::Assets::Surface         m_cacheSurface;
    RDGE::Texture         m_cacheTexture;
    RDGE::Graphics::Point m_finalDestination;
};

} // namespace GameObjects
} // namespace RDGE
