//! \headerfile <rdge/assets/bitmap_font.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Surface;
class Texture;
//!@}

struct glyph
{
    uint32       id = 0; //!< Character id
    screen_rect  clip;   //!< Spritesheet clipping rectangle
    math::uivec2 size;   //!< Spritesheet size (in pixels)
    tex_coords   coords; //!< Normalized texture coordinates

    uint32       page = 0;
    math::vec2   offset;
    float        x_advance = 0.f;
    // TODO kerning
};

class BitmapFont
{
public:
    explicit BitmapFont (const char* filepath);

    //! \brief BitmapFont dtor
    ~BitmapFont (void) noexcept = default;

    //!@{ Non-copyable and move enabled
    BitmapFont (const BitmapFont&) = delete;
    BitmapFont& operator= (const BitmapFont&) = delete;
    BitmapFont (BitmapFont&&) noexcept = default;
    BitmapFont& operator= (BitmapFont&&) noexcept = default;
    //!@}

    //! \brief BitmapFont Subscript Operator
    //! \details Retrieves glyph by character id
    //! \param [in] id Id of the character
    //! \returns Associated glyph
    //! \throws rdge::Exception Lookup failed
    const glyph& operator[] (uint32 id) const;

    float pad_top = 0.f;
    float pad_right = 0.f;
    float pad_bottom = 0.f;
    float pad_left = 0.f;
    float line_height = 0;

    std::vector<std::shared_ptr<Surface>> surfaces; //!< Surface created from image
    std::vector<std::shared_ptr<Texture>> textures; //!< Texture generated from the surface

private:
    std::vector<glyph> m_glyphs;
};

} // namespace rdge
