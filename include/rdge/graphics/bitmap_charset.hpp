//! \headerfile <rdge/graphics/bitmap_charset.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 03/10/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/vec2.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class BitmapFont;
class SpriteBatch;
class OrthographicCamera;
//!@}

//! \struct glyph
//! \brief Texture data for a single glyph
struct glyph
{
    tex_coords uvs;    //!< Texture coordinates of the glyph
    math::vec2 size;   //!< Glyph size (in pixels)
    math::vec2 offset; //!< Position offset when drawing (in pixels)
    float x_advance;   //!< Amount to advance position after drawing (in pixels)
    int8 page = -1;    //!< Texture index containing the glyph
};

//! \class BitmapCharset
//! \brief Character set for rendering text
//! \details Generates renderable components from a \ref BitmapFont, which
//!          includes a table of glyphs that map to character code points
//!          to generate glyph sprites for text rendering.
class BitmapCharset
{
public:
    //! \brief BitmapCharset ctor
    //! \param [in] font Font containing the font definition and image data
    //! \param [in] scale Asset to render scale
    explicit BitmapCharset (const BitmapFont& font, float scale = 1.f);

    //! \brief BitmapCharset default ctor/dtor
    BitmapCharset (void) = default;
    ~BitmapCharset (void) noexcept = default;

    //!@{ Non-copyable, move enabled
    BitmapCharset (const BitmapCharset&) = delete;
    BitmapCharset& operator= (const BitmapCharset&) = delete;
    BitmapCharset (BitmapCharset&&) noexcept = default;
    BitmapCharset& operator= (BitmapCharset&&) noexcept = default;
    //!@}

    //! \brief Draw the sprite using the provided renderer
    //! \param [in] renderer Render target
    //! \param [in] text Text to draw
    //! \param [in] pos Position to draw text
    void Draw (SpriteBatch& renderer, const std::string& text, const math::vec2& pos);

public:
    rdge::color color = color::WHITE; //!< Text color
    Shader shader; //!< Distance field font shader

private:
    std::vector<glyph> m_glyphs;     //!< List of renderable glyphs
    std::vector<Texture> m_textures; //!< List of textures containing the glyphs
};
#if 0
class SpriteBatchShader
{
protected:
    virtual std::string GetSource (ShaderType type);
};

class DistanceFieldShader : public SpriteBatchShader
{
public:
protected:
    std::string GetSource (ShaderType type) override;
};

class DistanceFieldOutlineShader : public SpriteBatchShader
{
public:
    void SetColor (const color& c);
    void SetOutlineDistance (float distance);
protected:
    std::string GetSource (ShaderType type) override;
};

class DistanceFieldDropShadowShader : public SpriteBatchShader
{
public:
protected:
    std::string GetSource (ShaderType type) override;
};
#endif

} // namespace rdge
