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

//! \struct glyph
//! \brief Represents the texture region of a specific glyph
struct glyph
{
    uint32      id = 0; //!< Character id
    screen_rect clip;   //!< Surface clipping rectangle
    tex_coords  coords; //!< Normalized texture coordinates
    math::vec2  size;   //!< Size of the glyph (in pixels)

    uint32      page = 0; //!< Index of the surface containing the glyph
    math::vec2  offset;
    float       x_advance = 0.f;

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

    std::vector<shared_asset<Surface>> surfaces; //!< Surfaces containing the glyphs
    std::vector<glyph> glyphs;                   //!< Glyph region list
};

} // namespace rdge
