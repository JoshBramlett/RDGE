//! \headerfile <rdge/assets/asset_pack.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/20/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/util/io/rwops_base.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace asset_pack {

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
    int32 channels;
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
    int64 offset;
    uint32 size;
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

class PackFile
{
public:
    //! \brief PackFile ctor
    //! \details Open an asset pack file for reading
    //! \param [in] filepath Path to pack file
    //! \throws rdge::Exception Invalid file
    //! \throws rdge::SDLException File cannot be loaded
    explicit PackFile (const char* filepath);

    //! \brief PackFile dtor
    ~PackFile (void) noexcept;

    //!@{ Non-copyable, move enabled
    PackFile (const PackFile&) = delete;
    PackFile& operator= (const PackFile&) = delete;
    PackFile (PackFile&&) noexcept;
    PackFile& operator= (PackFile&&) noexcept;
    //!@}

    Surface GetSurface (int32 asset_id);

private:
    rwops_base m_file;

    asset_pack::header      m_header;
    asset_pack::asset_info* m_table = nullptr;
};

} // namespace rdge
