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
ImportFonts (global_import_state& global_state)
{
    std::string path = global_state.parent_dir + FONT_DIR;
    std::cout << "ImportFonts from " << path << '\n';

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
            import.info.type = asset_type_font;
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
                    auto j = json::parse(text_data);
                    if (j.count("info") == 0 ||
                        j.count("common") == 0 ||
                        j.count("pages") == 0 ||
                        j.count("chars") == 0)
                    {
                        free(text_data);
                        throw std::runtime_error("Invalid font data file format");
                    }

                    auto& j_pages = j["pages"];
                    if (j_pages.size() > RDGE_MAX_FONT_PAGES)
                    {
                        free(text_data);
                        throw std::runtime_error("Font pages exceed maximum");
                    }

                    import.info.font.count = 0;
                    auto page_w = j["common"]["scaleW"].get<int32>();
                    auto page_h = j["common"]["scaleH"].get<int32>();
                    for (auto& j_page : j_pages)
                    {
                        auto image_file = j_page["file"].get<std::string>();
                        auto image_name = rdge::basename(rdge::remove_extension(image_file));
                        uint32 table_id = global_state.get_id(image_name, asset_type_surface);
                        if (table_id == global_import_state::INVALID_TABLE_ID)
                        {
                            free(text_data);
                            throw std::runtime_error("Unknown image: " + image_name);
                        }

                        // sanity check - all pages will have the same dimensions
                        const auto& image_asset = global_state.imported_assets[table_id];
                        if (table_id != image_asset.table_id ||
                            page_w != image_asset.info.surface.width ||
                            page_h != image_asset.info.surface.height)
                        {
                            free(text_data);
                            throw std::runtime_error("Image mismatch: " + image_name);
                        }

                        import.info.font.surface_ids[import.info.font.count++] = table_id;
                        j_page["image_table_id"] = table_id;
                    }

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
                              << " surface_ids=" << import.info.font.count
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
        std::cout << "  Subdirectory " << FONT_DIR << " not found\n";
    }

    return result;
}
