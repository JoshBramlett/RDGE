//! \headerfile <rdge/graphics/text.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/19/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/color.hpp>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Text
//! \brief Renderable text
class Text : public Renderable2D
{
public:
    //! \brief Text ctor
    //! \param [in] text Text to render
    //! \param [in] x x-coordinate location
    //! \param [in] y y-coordinate location
    //! \param [in] font Font to render text
    //! \param [in] color Color of all vertices
    //! \param [in] mode Font render mode
    explicit Text (
                   std::string                         text,
                   float                               x,
                   float                               y,
                   std::shared_ptr<RDGE::Assets::Font> font,
                   const RDGE::Color&                  color,
                   RDGE::Assets::Font::RenderMode mode = RDGE::Assets::Font::RenderMode::Solid
                  );

    // TODO: Add copy, move, and destructor

    //! \brief Set the text
    //! \param [in] text Text to render
    //! \note Texutre and size data will be reset
    void SetText (const std::string& text);

    //! \brief Set the color of the text
    //! \param [in] color Color to render the text
    //! \param [in] ignore_alpha False will only set the RGB values
    //! \note Texutre and size data will be reset
    virtual void SetColor (const RDGE::Color& color, bool ignore_alpha = true) override;

private:

    //! \brief Rebuild the label
    //! \details Creates a surface image based on the cached members and
    //!          converts it to a renderable object.
    void Rebuild (void);

    std::string                         m_text;
    std::shared_ptr<RDGE::Assets::Font> m_font;
    RDGE::Assets::Font::RenderMode      m_renderMode;
};

} // namespace Graphics
} // namespace RDGE
