//! \headerfile <rdge/assets/bitmap_font.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class PackFile;
//!@}

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

//! \class BitmapFont
//! \brief Font made from pre-built glyphs stored within an image asset
//! \details Bitmap fonts are externally generated and define how the image
//!          asset(s) should be rendered to format text.  The data file defining
//!          the font can either be a bmfont formatted text file, or a
//!          proprietary json format.
//!
//!          The proprietary json format expands on the bmfont format.
//!
//! \code{.json}
//! {
//!   "info": {
//!     "aa": 1,
//!     "bold": true,
//!     "spacing": {
//!       "w": -4,
//!       "h": -4
//!     },
//!     "charset": "",
//!     "smooth": true,
//!     "stretchH": 100,
//!     "face": "BitPotionExt",
//!     "padding": {
//!       "top": 2,
//!       "right": 2,
//!       "left": 2,
//!       "bottom": 2
//!     },
//!     "italic": true,
//!     "unicode": true,
//!     "size": 32
//!   },
//!   "common": {
//!     "scaleW": 256,
//!     "base": 24,
//!     "lineHeight": 32,
//!     "scaleH": 256,
//!     "pages": 1,
//!     "packed": 0
//!   },
//!   "pages": [
//!     {
//!       "id": 0,
//!       "file": "../images/bitpotion.png"
//!     }
//!   ],
//!   "chars": [
//!     {
//!       "chnl": 0,
//!       "height": 26,
//!       "width": 13,
//!       "xadvance": 12,
//!       "y": 0,
//!       "x": 0,
//!       "yoffset": 0,
//!       "id": 0,
//!       "xoffset": -1,
//!       "page": 0
//!     },
//!     ...
//!   ],
//!   "effects": [
//!     {
//!       "color": "ffffff",
//!       "spread": 2.0,
//!       "scale": 32,
//!       "class": "DistanceFieldEffect"
//!     }
//!   ],
//! }
//! \endcode
class BitmapFont
{
public:
    //! \brief BitmapFont ctor
    //! \details Loads and parses the bmfont file.
    //! \param [in] filepath Path to the font file
    //! \throws rdge::Exception Unable to parse font
    explicit BitmapFont (const char* filepath);

    //! \brief BitmapFont ctor
    //! \details Loads and parses the packed json (used with \ref PackFile).
    //! \param [in] msgpack Packed json configuration
    //! \param [in] packfile \ref PackFile reference (to load dependencies)
    //! \throws rdge::Exception Unable to parse font
    //! \see http://msgpack.org/
    explicit BitmapFont (const std::vector<uint8>& msgpack, PackFile& packfile);

    //!@{ BitmapFont default ctor/dtor
    BitmapFont (void) = default;
    ~BitmapFont (void) noexcept = default;
    //!@}

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

    //!@{ BitmapFont properties
    bool IsUnicode (void) const noexcept;
    bool IsBold (void) const noexcept;
    bool IsItalic (void) const noexcept;
    bool IsDistanceField (void) const noexcept;
    //!@}

    std::string name;        //!< Name of the TTF
    size_t size = 0;         //!< Size of the TTF
    float line_height = 0.f; //!< Distance between each line of text (in pixels)
    float baseline = 0.f;    //!< Distance between the top and the baseline (in pixels)

    //! \struct distance_field_effect_data
    //! \brief Settings if the font was build with a distance field effect
    struct distance_field_effect_data
    {
        rdge::color color;
        size_t scale = 0;
        float spread = 0.f;
    } distance_field;

    std::vector<shared_asset<Surface>> surfaces; //!< Surfaces containing the glyphs
    std::vector<glyph_region> glyphs;            //!< Glyph region list

private:
    uint8 m_flags = 0;
};

} // namespace rdge
