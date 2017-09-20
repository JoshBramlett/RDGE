//! \headerfile <rdge/assets/file_format.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/13/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace asset_pack {

//! {
//!     "image_path": "textures/image.png",
//!     "image_scale": 4,
//!     "texture_parts": [ {
//!         "name": "part_name",
//!         "x": 0,
//!         "y": 0,
//!         "width": 32,
//!         "height": 32,
//!         "origin": [ 16, 16 ]
//!     } ],
//!     "animations": [ {
//!         "name": "animation_name",
//!         "mode": "normal",
//!         "interval": 100,
//!         "frames": [ {
//!             "name": "part_name",
//!             "flip": "horizontal"
//!         } ]
//!     } ]
//! }

#define MAGIC_VALUE_CODE(a, b, c, d) (((uint32)(a) << 0) |  \
                                      ((uint32)(b) << 8) |  \
                                      ((uint32)(c) << 16) | \
                                      ((uint32)(d) << 24))

#define RDGE_MAGIC_VALUE MAGIC_VALUE_CODE('r','d','g','e')
#define RDGE_ASSET_PACK_VERSION 0

enum asset_type
{
    asset_type_invalid = 0,
    asset_type_surface,
    asset_type_font,
    asset_type_spritesheet,
    asset_type_tilemap,
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
    int32 width;
    int32 height;
    int32 format;
};

struct spritesheet_info
{
    uint32 surface_id;
};

struct tilemap_info
{
    uint32 surface_id;
};

struct asset_info
{
    uint64 offset;
    asset_type type;

    union
    {
        surface_info     surface;
        spritesheet_info spritesheet;
        tilemap_info     tilemap;
    };
};

#pragma pack(pop)


} // namespace asset_pack
} // namespace rdge
