//! \headerfile <rdge/assets/bmfont.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 07/31/2017

#pragma once

#include <rdge/core.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

enum bmfont_info_flags
{
    bmfont_info_smooth  = 0x0001, //!< Smoothing turned on
    bmfont_info_unicode = 0x0002, //!< Unicode charset
    bmfont_info_italic  = 0x0004, //!< Italic font
    bmfont_info_bold    = 0x0008  //!< Bold font
};

//! \struct bmfont_info
//! \brief Contains information on how the font was generated
struct bmfont_info
{
    std::string face;    //!< Name of the TTF
    std::string charset; //!< Name of the OEM charset (when not unicode)
    int32 size = 0;      //!< Size of the TTF
    int32 stretchH = 0;  //!< Font height stretch percentage (100 means no stretch)
    int32 aa = 0;        //!< Supersampling level (1 means no supersampling used)
    int32 outline = 0;   //!< Outline thickness
    int32 padding[4] = { 0, 0, 0, 0 }; //!< Character padding (top, right, bottom, left)
    int32 spacing[2] = { 0, 0 };       //!< Character spacing (horizontal, vertical)
    uint32 flags = 0;                  //!< \ref bmfont_info_flags bitset
};

enum bmfont_common_flags
{
    //! \brief Monochrome characters are packed into each of the texture channels
    //! \details Alpha channel describes what is stored in each channel.
    bmfont_common_packed = 0x0001
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
    int32 lineHeight = 0; //!< Distance between each line of text (in pixels)
    int32 base = 0;       //!< Distance between the top and the baseline (in pixels)
    int32 scaleW = 0;     //!< Texture width
    int32 scaleH = 0;     //!< Texture height
    int32 pages = 0;      //!< Number of texture pages
    int32 alphaChnl = 0;
    int32 redChnl = 0;
    int32 greenChnl = 0;
    int32 blueChnl = 0;
    uint32 flags = 0;
};

//! \struct bmfont_page
//! \brief Texture file containing character glyphs
struct bmfont_page
{
    std::string file;
    int32 id = 0;
};

//! \struct bmfont_char
//! \brief Character data inluding texture lookup and drawing info
struct bmfont_char
{
    uint32 id = 0;      //!< Character id
    uint32 x = 0;       //!< Left clip of the character texture region
    uint32 y = 0;       //!< Top clip of the character texture region
    uint32 width = 0;   //!< Width of the character texture region
    uint32 height = 0;  //!< Height of the character texture region
    int32 xoffset = 0;  //!< X-axis amount to offset the cursor position
    int32 yoffset = 0;  //!< Y-axis amount to offset the cursor position
    int32 xadvance = 0; //!< Amount to advance the cursor after drawing
    uint32 page = 0;    //!< Index of the texture page where the character image is found

    //! \brief Texture channel where the character image is found
    //! \details (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels)
    uint32 chnl = 0;
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

    uint32 high_id = 0; //!< Highest character id in the set
};

//! \brief Load and populate bmfont definition from file
//! \param [in] filepath File containing bmfont definition
//! \param [out] font bmfont to populate
//! \throws rdge::Exception Import failed
void
load_bmfont (const char* filepath, bmfont_data& font);

} // namespace rdge
