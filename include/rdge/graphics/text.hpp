//! \headerfile <rdge/graphics/text.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/23/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/math/vec3.hpp>

#include <string>
#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Text
//! \brief Renderable text (label)
class Text : public ISprite
{
public:
    //! \brief Text ctor
    //! \details Creates a texture based upon the provided values which can be used
    //!          to send to a render target.
    //! \param [in] text Text to render
    //! \param [in] pos Position of the text
    //! \param [in] font Text font
    //! \param [in] color Text color
    //! \param [in] mode Font render mode
    explicit Text (std::string           text,
                   const math::vec3&     pos,
                   std::shared_ptr<Font> font,
                   const color&          color,
                   Font::RenderMode      mode = Font::RenderMode::SOLID);

    //! \brief Text dtor
    ~Text (void) noexcept = default;

    //!@{
    //! \brief Non-copyable and move enabled
    Text (const Text&) = delete;
    Text& operator= (const Text&) = delete;
    Text (Text&&) noexcept = default;
    Text& operator= (Text&&) noexcept = default;
    //!@}

    //! \brief Register the texture with the render target
    //! \details This step must be performed prior to making a draw call.
    //! \param [in] renderer Render target
    void SetRenderTarget (SpriteBatch& renderer) override;

    //! \brief Draw the sprite using the provided renderer
    //! \param [in] renderer Render target
    void Draw (SpriteBatch& renderer) override;

    //! \brief Set the sprites depth value
    //! \param [in] depth Depth (z-index) value
    void SetDepth (float depth) override;

    //! \brief Set the text
    //! \param [in] text Text to render
    //! \note Texutre and size data will be reset
    void SetText (const std::string& text);

    //! \brief Set the color of the text
    //! \param [in] color Color to render the text
    //! \note Texutre and size data will be reset
    void SetColor (const color& color);

public:
    SpriteVertices vertices; //!< Array of vertex attribute data

private:

    //! \brief Rebuild the text label
    //! \details Creates a surface image based on the cached members and
    //!          converts it to a renderable object.  Members are cached because
    //!          if any changes a new texture needs to be generated.
    void Rebuild (void);

    std::string              m_text;       //!< Cached text string
    color                    m_color;      //!< Cached text color
    Font::RenderMode         m_renderMode; //!< Cached render mode
    std::shared_ptr<Font>    m_font;       //!< Font used to render text
    std::shared_ptr<Texture> m_texture;    //!< Texture dynamically generated
};

} // namespace rdge
