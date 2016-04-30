//! \headerfile <rdge/graphics/label.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/19/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/color.hpp>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Label
//! \brief Renderable text
class Label : public Renderable2D
{
public:
    //! \brief Label ctor
    //! \param [in] x x-coordinate location
    //! \param [in] y y-coordinate location
    //! \param [in] font Font to render text
    //! \param [in] color Color of all vertices
    explicit Label (const std::string& text, float x, float y, RDGE::Assets::Font font, const RDGE::Color& color);

    void SetText (const std::string& text);

private:
    RDGE::Assets::Font m_font;
};

} // namespace Graphics
} // namespace RDGE
