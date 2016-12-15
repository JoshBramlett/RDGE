//! \headerfile <rdge/gfx/text.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/19/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/gfx/color.hpp>
#include <rdge/gfx/renderable2d.hpp>
#include <rdge/assets/font.hpp>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

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
                   std::shared_ptr<rdge::assets::Font> font,
                   const color&                        color,
                   rdge::assets::Font::RenderMode mode = rdge::assets::Font::RenderMode::SOLID
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
    virtual void SetColor (const color& color, bool ignore_alpha = true) override;

private:

    //! \brief Rebuild the label
    //! \details Creates a surface image based on the cached members and
    //!          converts it to a renderable object.
    void Rebuild (void);

    std::string                         m_text;
    std::shared_ptr<rdge::assets::Font> m_font;
    rdge::assets::Font::RenderMode      m_renderMode;
};

} // namespace gfx
} // namespace rdge
