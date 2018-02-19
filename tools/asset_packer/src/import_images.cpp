#include "common.hpp"

#include <rdge/util/strings.hpp>

#include <iostream>

#define TINYFILES_IMPL
#include <tinyheaders/tinyfiles.h>

#define STB_IMAGE_IMPLEMENTATION
#include <nothings/stb_image.h>

using namespace rdge;
using namespace rdge::asset_pack;

namespace {

bool
IsExtensionSupported (const std::string& file)
{
    static std::vector<std::string> SUPPORTED = { "png" };

    for (const auto& s : SUPPORTED)
    {
        if (rdge::ends_with(file, s))
        {
            return true;
        }
    }

    return false;
}

} // anonymous namespace

import_result
ImportImages (global_import_state& global_state)
{
    std::string path = global_state.parent_dir + IMAGE_DIR;
    std::cout << "ImportImages from " << path << '\n';

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
            import.info.type = asset_type_surface;
            import.info.offset = global_state.running_offset;
            import.name = rdge::remove_extension(file.name);
            import.table_id = global_state.running_count;

            if (global_state.is_unique(import))
            {
                import.data = stbi_load(file.path,
                                        &import.info.surface.width,
                                        &import.info.surface.height,
                                        &import.info.surface.channels,
                                        0);
                if (import.data)
                {
                    import.info.size = import.info.surface.width *
                                       import.info.surface.height *
                                       import.info.surface.channels;

                    std::cout << " SUCCESS"
                              << " [" << import.info.surface.width
                              << "x" << import.info.surface.height << "]"
                              << " channels=" << import.info.surface.channels
                              << " file_size=" << file.size
                              << " import_size=" << import.info.size << std::endl;

                    global_state.imported_assets.push_back(import);
                    global_state.running_count++;
                    global_state.running_offset += import.info.size;
                    result.success++;
                }
                else
                {
                    std::cout << " FAILED stbi_load"
                              << " reason=" << stbi_failure_reason() << std::endl;
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
        std::cout << "  Subdirectory " << IMAGE_DIR << " not found\n";
    }

    return result;
}
