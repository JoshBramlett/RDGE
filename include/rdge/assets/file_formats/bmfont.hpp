//! \headerfile <rdge/assets/file_formats/bmfont.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 07/31/2017

#pragma once

#include <rdge/core.hpp>

#include <vector>

//!@{ Forward declarations
#include <nlohmann/json_fwd.hpp>
namespace rdge {
namespace tilemap { class Tilemap; }
}
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

enum bmfont_info_flags
{
    bmfont_info_smooth       = 0x01, //!< Smoothing turned on
    bmfont_info_unicode      = 0x02, //!< Unicode charset
    bmfont_info_italic       = 0x04, //!< Italic font
    bmfont_info_bold         = 0x08, //!< Bold font
    bmfont_info_fixed_height = 0x10, //!< Undocumented
    bmfont_info_reserved_1   = 0x20,
    bmfont_info_reserved_2   = 0x40,
    bmfont_info_reserved_3   = 0x80
};

//! \struct bmfont_info
//! \brief Contains information on how the font was generated
struct bmfont_info
{
    int16 size = 0;      //!< Size of the TTF
    uint8 flags = 0;     //!< \ref bmfont_info_flags bitset
    std::string charset; //!< Name of the OEM charset (when not unicode)
    uint16 stretchH = 0; //!< Font height stretch percentage (100 means no stretch)
    uint8 aa = 0;        //!< Supersampling level (1 means no supersampling used)
    uint8 padding[4] = { 0, 0, 0, 0 }; //!< Character padding (top, right, bottom, left)
    int8 spacing[2] = { 0, 0 };        //!< Character spacing (horizontal, vertical)
    uint8 outline = 0;   //!< Outline thickness
    std::string face;    //!< Name of the TTF
};

enum bmfont_common_flags
{
    //! \brief Monochrome characters are packed into each of the texture channels
    //! \details Alpha channel describes what is stored in each channel.
    bmfont_common_reserved_1 = 0x01,
    bmfont_common_reserved_2 = 0x02,
    bmfont_common_reserved_3 = 0x04,
    bmfont_common_reserved_4 = 0x08,
    bmfont_common_reserved_5 = 0x10,
    bmfont_common_reserved_6 = 0x20,
    bmfont_common_reserved_7 = 0x40,
    bmfont_common_packed     = 0x80
};

enum bmfont_common_channel_type
{
    bmfont_common_channel_glyph             = 0, //!< Channel holds glyph data
    bmfont_common_channel_outline           = 1, //!< Channel holds outline data
    bmfont_common_channel_glyph_and_outline = 2, //!< Channel holds glyph and outline data
    bmfont_common_channel_zero              = 3, //!< Channel set to zero
    bmfont_common_channel_one               = 4  //!< Channel set to one
};

//! \struct bmfont_common
//! \brief Contains information common to all characters
struct bmfont_common
{
    uint16 lineHeight = 0; //!< Distance between each line of text (in pixels)
    uint16 base = 0;       //!< Distance between the top and the baseline (in pixels)
    uint16 scaleW = 0;     //!< Texture width
    uint16 scaleH = 0;     //!< Texture height
    uint16 pages = 0;      //!< Number of texture pages
    uint8 flags = 0;
    uint8 alphaChnl = 0;
    uint8 redChnl = 0;
    uint8 greenChnl = 0;
    uint8 blueChnl = 0;
};

//! \struct bmfont_page
//! \brief Texture file containing character glyphs
struct bmfont_page
{
    std::string file;
    int32 id = 0;
    size_t image_table_id = 0;
};

//! \struct bmfont_char
//! \brief Character data inluding texture lookup and drawing info
struct bmfont_char
{
    uint32 id = 0;      //!< Character id
    uint16 x = 0;       //!< Left clip of the character texture region
    uint16 y = 0;       //!< Top clip of the character texture region
    uint16 width = 0;   //!< Width of the character texture region
    uint16 height = 0;  //!< Height of the character texture region
    int16 xoffset = 0;  //!< X-axis amount to offset the cursor position
    int16 yoffset = 0;  //!< Y-axis amount to offset the cursor position
    int16 xadvance = 0; //!< Amount to advance the cursor after drawing
    uint8 page = 0;    //!< Index of the texture page where the character image is found

    //! \brief Texture channel where the character image is found
    //! \details (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels)
    uint8 chnl = 0;
};

//! \struct bmfont_kerning
//! \brief Kerning information to adjust distance between characters
struct bmfont_kerning
{
    uint32 first = 0;  //!< First character id
    uint32 second = 0; //!< Second character id
    int32 amount = 0;  //!< Amount to adjust x-axis when drawing second character after first
};

//! \struct bmfont_data
//! \brief Container for bmfont imported data
//! \details Format is based on the bmfont spec, also used by libGDX.
//! \see http://www.angelcode.com/products/bmfont/doc/file_format.html
struct bmfont_data
{
    bmfont_info info;
    bmfont_common common;

    std::vector<bmfont_page> pages;
    std::vector<bmfont_char> chars;
    std::vector<bmfont_kerning> kerning_table;

    //! \brief Highest character id in the set
    //! \details Calculated when importing using the native bmfont, but should
    //!          be provided when importing using the modified json format.
    uint32 high_id = 0;
};

//! \brief Load and populate bmfont definition from file
//! \param [in] filepath File containing bmfont definition
//! \param [out] font bmfont to populate
//! \note Structures closely resemble the binary file layout, but currently
//!       only the text file format is supported.
//! \throws rdge::Exception Import failed
void
load_bmfont (const char* filepath, bmfont_data& font);

//! \brief Load and populate bmfont definition from json
//! \param [in] j json formatted bmfont
//! \param [out] font bmfont to populate
//! \throws rdge::Exception Import failed
void
load_bmfont (const nlohmann::json& j, bmfont_data& font);

} // namespace rdge
