//! \headerfile <rdge/graphics/text.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/23/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/assets/bitmap_font.hpp>

#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SpriteBatch;
//!@}

//! \class Text
//! \brief Renderable text (label)
class Text
{
public:
    //! \brief Text ctor
    //! \details Creates a texture based upon the provided values which can be used
    //!          to send to a render target.
    //! \param [in] font Text font
    explicit Text (shared_asset<BitmapFont> font);

    //! \brief Text default ctor/dtor
    Text (void) = default;
    ~Text (void) noexcept = default;

    //!@{ Non-copyable, move enabled
    Text (const Text&) = delete;
    Text& operator= (const Text&) = delete;
    Text (Text&&) noexcept = default;
    Text& operator= (Text&&) noexcept = default;
    //!@}

    //! \brief Draw the sprite using the provided renderer
    //! \param [in] renderer Render target
    //! \param [in] text Text to draw
    //! \param [in] pos Position to draw text
    void Draw (SpriteBatch& renderer, const std::string& text, const math::vec2& pos);

public:
    rdge::color color = color::WHITE; //!< Text color

private:
    shared_asset<BitmapFont> m_font; //!< Text font
    std::vector<Texture> m_textures; //!< List of textures containing the glyphs
};

} // namespace rdge
