#include "common.hpp"

#include <rdge/util/strings.hpp>
#include <rdge/util/io/rwops_base.hpp>

#include <iostream>
#include <sstream>

#include <tinyheaders/tinyfiles.h>

using namespace rdge;
using namespace rdge::asset_pack;
using json = nlohmann::json;

namespace {

bool
IsExtensionSupported (const std::string& file)
{
    return rdge::ends_with(file, "json");
}

} // anonymous namespace

import_result
ImportTilesets (global_import_state& global_state)
{
    std::string path = global_state.parent_dir + TILESET_DIR;
    std::cout << "ImportTilesets from " << path << '\n';

    import_result result;

    tfDIR dir;
    if (tfDirOpen(&dir, path.c_str()))
    {
        while (dir.has_next)
        {
            tfFILE file;
            tfReadFile(&dir, &file);

            if (file.is_dir)
            {
                tfDirNext(&dir);
                continue;
            }

            if (!file.is_reg || !IsExtensionSupported(file.name))
            {
                std::cout << "  Skipping [" << file.name << "] unsupported type\n";
                result.skipped++;
                continue;
            }

            std::cout << "  Processing [" << file.name << "]";

            imported_asset import;
            import.info.type = asset_type_tileset;
            import.info.offset = global_state.running_offset;
            import.name = rdge::remove_extension(file.name);
            import.table_id = global_state.running_count;

            if (global_state.is_unique(import))
            {
                try
                {
                    auto rwops = rwops_base::from_file(file.path, "rt");
                    auto text_size = rwops.size();
                    char* text_data = (char*)calloc(text_size + 1, sizeof(char));
                    if (!text_data)
                    {
                        throw std::runtime_error("failed memory allocation");
                    }

                    rwops.read(text_data, text_size);
                    const auto j = json::parse(text_data);
                    if (j["type"].get<std::string>() != "tileset")
                    {
                        free(text_data);
                        throw std::runtime_error("Invalid tileset format");
                    }

                    auto image_file = j["image"].get<std::string>();
                    auto image_name = rdge::basename(rdge::remove_extension(image_file));
                    uint32 table_id = global_state.get_id(image_name, asset_type_surface);
                    if (table_id == global_import_state::INVALID_TABLE_ID)
                    {
                        free(text_data);
                        throw std::runtime_error("Unknown image: " + image_name);
                    }

                    // sanity check
                    const auto& image_asset = global_state.imported_assets[table_id];
                    if (table_id != image_asset.table_id ||
                        j["imagewidth"].get<int32>() != image_asset.info.surface.width ||
                        j["imageheight"].get<int32>() != image_asset.info.surface.height)
                    {
                        free(text_data);
                        throw std::runtime_error("Image mismatch: " + image_name);
                    }

                    import.info.tileset.surface_id = table_id;
                    std::vector<uint8> msgpack = json::to_msgpack(j);

                    free(text_data);
                    import.data = malloc(msgpack.size());
                    if (!import.data)
                    {
                        throw std::runtime_error("failed memory allocation");
                    }

                    memcpy(import.data, msgpack.data(), msgpack.size());
                    import.info.size = msgpack.size();

                    std::cout << " SUCCESS"
                              << " surface_id=" << import.info.tileset.surface_id
                              << " file_size=" << file.size
                              << " import_size=" << import.info.size << std::endl;

                    global_state.imported_assets.push_back(import);
                    global_state.running_count++;
                    global_state.running_offset += import.info.size;
                    result.success++;
                }
                catch (const std::exception& ex)
                {
                    std::cout << " FAILED on exception"
                              << " reason=" << ex.what() << std::endl;
                    result.failed++;
                }
                catch (...)
                {
                    std::cout << " FAILED on unknown exception\n";
                    result.failed++;
                }
            }
            else
            {
                std::cout << " FAILED asset not unique\n";
                result.failed++;
            }

            tfDirNext(&dir);
        }

        tfDirClose(&dir);
    }
    else
    {
        std::cout << "  Subdirectory " << TILESET_DIR << " not found\n";
    }

    return result;
}
