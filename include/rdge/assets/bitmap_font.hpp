//! \headerfile <rdge/assets/bitmap_font.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct glyph_region
//! \brief Represents the texture region of a specific glyph
struct glyph_region
{
    uint32 id = 0;     //!< Character id
    screen_rect clip;  //!< Surface clipping rectangle
    tex_coords coords; //!< Normalized texture coordinates
    math::vec2 size;   //!< Size of the glyph (in pixels)

    math::vec2 offset;     //!< Position offset when drawing (in pixels)
    float x_advance = 0.f; //!< Amount to advance position after drawing (in pixels)
    uint32 page = 0;       //!< Surface index containing the glyph

    // TODO kerning
};

class BitmapFont
{
public:
    explicit BitmapFont (const char* filepath);

    //! \brief BitmapFont dtor
    ~BitmapFont (void) noexcept = default;

    //!@{ Non-copyable, move enabled
    BitmapFont (const BitmapFont&) = delete;
    BitmapFont& operator= (const BitmapFont&) = delete;
    BitmapFont (BitmapFont&&) noexcept = default;
    BitmapFont& operator= (BitmapFont&&) noexcept = default;
    //!@}

    //! \brief BitmapFont Subscript Operator
    //! \details Retrieves glyph_region by character id
    //! \param [in] id Id of the character
    //! \returns Associated glyph_region
    //! \throws rdge::Exception Lookup failed
    const glyph_region& operator[] (uint32 id) const;

    float pad_top = 0.f;
    float pad_right = 0.f;
    float pad_bottom = 0.f;
    float pad_left = 0.f;
    float line_height = 0.f;
    float baseline = 0.f;

    std::vector<shared_asset<Surface>> surfaces; //!< Surfaces containing the glyphs
    std::vector<glyph_region> glyphs;            //!< Glyph region list
};

} // namespace rdge
