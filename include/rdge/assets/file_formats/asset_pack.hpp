//! \headerfile <rdge/assets/file_formats/asset_pack.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/20/2017

#pragma once

#include <rdge/core.hpp>

#define MAGIC_VALUE_CODE(a, b, c, d) (((uint32)(a) << 0) |  \
                                      ((uint32)(b) << 8) |  \
                                      ((uint32)(c) << 16) | \
                                      ((uint32)(d) << 24))

#define RDGE_MAGIC_VALUE MAGIC_VALUE_CODE('r','d','g','e')
#define RDGE_ASSET_PACK_VERSION 0

#define RDGE_MAX_FONT_PAGES 4

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace asset_pack {

enum asset_type
{
    asset_type_invalid = 0,
    asset_type_surface,
    asset_type_font,
    asset_type_spritesheet,
    asset_type_tilemap,
    asset_type_tileset,
    asset_type_sound
};

#pragma pack(push, 1)

struct header
{
    uint32 magic_value;
    uint32 version;

    uint32 asset_count;
    uint32 assets;
};

struct surface_info
{
    // stbi requires all are signed types
    int32 width;
    int32 height;
    int32 channels;
};

struct font_info
{
    uint32 surface_ids[RDGE_MAX_FONT_PAGES];
    size_t count;
};

struct spritesheet_info
{
    uint32 surface_id;
};

struct tilemap_info
{
    uint32 empty;
};

struct tileset_info
{
    uint32 surface_id;
};

struct asset_info
{
    int64 offset;
    uint32 size;
    asset_type type;

    union
    {
        surface_info     surface;
        font_info        font;
        spritesheet_info spritesheet;
        tilemap_info     tilemap;
        tileset_info     tileset;
    };
};

#pragma pack(pop)

//! \brief asset_type stream output operator
std::ostream& operator<< (std::ostream&, asset_type);

} // namespace asset_pack

//! \brief asset_type string conversion
std::string to_string (asset_pack::asset_type);

} // namespace rdge
