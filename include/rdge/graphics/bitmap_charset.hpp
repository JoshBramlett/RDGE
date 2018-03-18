//! \headerfile <rdge/graphics/bitmap_charset.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 03/10/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/shaders/sprite_batch_shader.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/mat4.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class BitmapCharset;
class GlyphLayout;
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

//! \class GlyphLayout
//! \brief Cache of sprite data for quick text rendering
//! \details Generates a collection of sprites from a string of characters,
//!          allowing the sprites to be generated once and re-used when rendering.
class GlyphLayout
{
public:
    //! \brief GlyphLayout ctor
    //! \param [in] text Text to render
    //! \param [in] pos Position in render space
    //! \param [in] charset Character set (aka font)
    //! \param [in] scale Scale to apply to the glyphs
    //! \param [in] color Default rendering color
    //! \param [in] wrap_width Pixels until text is wrapped onto a new line
    //! \param [in] depth Depth to apply to the sprite data
    GlyphLayout (std::string text,
                 const math::vec2& pos,
                 const BitmapCharset& charset,
                 float scale,
                 const rdge::color& color,
                 float wrap_width,
                 float depth);

    //!@{ GlyphLayout default ctor/dtor
    GlyphLayout (void) = default;
    ~GlyphLayout (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    GlyphLayout (const GlyphLayout&) = delete;
    GlyphLayout& operator= (const GlyphLayout&) = delete;
    GlyphLayout (GlyphLayout&&) noexcept = default;
    GlyphLayout& operator= (GlyphLayout&&) noexcept = default;
    //!@}

    //!@{ Property setters (may rebuild if required)
    void SetText (std::string, const BitmapCharset&);
    void SetPosition (const math::vec2&);
    void SetDefaultColor (const rdge::color&);
    void SetDepth (float);
    //!@}

    size_t line_count = 0; //!< Number of rendered lines
    math::vec2 size;       //!< Size of the rendering region

    std::vector<sprite_data> sprites; //!< Sprite data

private:
    friend class BitmapCharset;

    void Rebuild (const BitmapCharset&);

    std::string m_text; //!< Raw text
    math::mat4 m_xform; //!< Position translation

    rdge::color m_color = rdge::color::WHITE; //!< Default text color
    float m_scale = 1.f;                      //!< Scale applied to sprites in local space
    float m_wrap = 0.f;                       //!< Pixel count for word wrap
    float m_depth = 0.f;                      //!< Depth applied to sprites
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

    //!@{ BitmapCharset default ctor/dtor
    BitmapCharset (void) = default;
    ~BitmapCharset (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    BitmapCharset (const BitmapCharset&) = delete;
    BitmapCharset& operator= (const BitmapCharset&) = delete;
    BitmapCharset (BitmapCharset&&) noexcept = default;
    BitmapCharset& operator= (BitmapCharset&&) noexcept = default;
    //!@}

    //! \brief Draw text using the provided renderer
    //! \param [in] renderer Render target
    //! \param [in] layout Layout containing the sprite data
    void Draw (SpriteBatch& renderer, const GlyphLayout& layout);

public:
    float line_height = 0.f; //!< Distance in pixels between each line of text
    float baseline = 0.f;    //!< Distance in pixels from the top to the baseline

    // TODO Could refactor to be a non-value type to take advantage of
    //      polymorphism.  Either that or remove the member and pass the shader
    //      into the draw function.
    DistanceFieldShader shader; //!< Distance field shader
    //DistanceFieldOutlineShader shader;
    //DistanceFieldDropShadowShader shader;

    std::vector<glyph> glyphs;     //!< List of renderable glyphs
    std::vector<Texture> textures; //!< List of textures containing the glyphs
};

} // namespace rdge
