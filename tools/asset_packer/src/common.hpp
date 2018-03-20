#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/util/json.hpp>

#include <string>
#include <vector>

#define IMAGE_DIR       "images"
#define FONT_DIR        "fonts"
#define SPRITESHEET_DIR "spritesheets"
#define TILEMAP_DIR     "tilemaps"
#define TILESET_DIR     "tilesets"

struct import_result
{
    size_t success = 0;
    size_t failed = 0;
    size_t skipped = 0;

    import_result& operator+= (const import_result& rhs)
    {
        success += rhs.success;
        failed += rhs.failed;
        skipped += rhs.skipped;

        return *this;
    }
};

struct imported_asset
{
    std::string name;
    rdge::uint32 table_id;
    void* data;

    rdge::asset_pack::asset_info info;

    nlohmann::json enums;
};

struct global_import_state
{
    static constexpr rdge::uint32 INVALID_TABLE_ID = std::numeric_limits<rdge::uint32>::max();

    // output
    std::string data_file;
    std::string header_file;

    // input
    std::string parent_dir;
    rdge::uint32 running_count = 0;     // running asset id
    rdge::uint64 running_offset = 0;    // running asset offset
    std::vector<imported_asset> imported_assets;

    rdge::uint32 get_id (const std::string& name, rdge::asset_pack::asset_type type) const
    {
        for (const auto& asset : imported_assets)
        {
            if (asset.info.type == type && asset.name == name)
            {
                return asset.table_id;
            }
        }

        return INVALID_TABLE_ID;
    }

    bool is_unique (const imported_asset& asset) const
    {
        return get_id(asset.name, asset.info.type) == INVALID_TABLE_ID;
    }
};

import_result ImportFonts (global_import_state&);
import_result ImportImages (global_import_state&);
import_result ImportSpritesheets (global_import_state&);
import_result ImportTilemaps (global_import_state&);
import_result ImportTilesets (global_import_state&);
