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
ImportSpritesheets (global_import_state& global_state)
{
    std::string path = global_state.parent_dir + SPRITESHEET_DIR;
    std::cout << "ImportSpritesheets from " << path << '\n';

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
            import.info.type = asset_type_spritesheet;
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
                    if (j.count("meta") == 0)
                    {
                        free(text_data);
                        throw std::runtime_error("Invalid spritesheet format");
                    }

                    if (j.count("frames") == 0)
                    {
                        free(text_data);
                        throw std::runtime_error("Spritesheet has no frame data");
                    }

                    auto& j_meta = j["meta"];
                    auto image_file = j_meta["image"].get<std::string>();
                    auto image_name = rdge::basename(rdge::remove_extension(image_file));
                    uint32 table_id = global_state.get_id(image_name, asset_type_surface);
                    if (table_id == global_import_state::INVALID_TABLE_ID)
                    {
                        free(text_data);
                        throw std::runtime_error("Unknown image: " + image_name);
                    }

                    // sanity check
                    const auto& image_asset = global_state.imported_assets[table_id];
                    const auto& image_size = j_meta["size"];
                    if (table_id != image_asset.table_id ||
                        image_size["w"].get<int32>() != image_asset.info.surface.width ||
                        image_size["h"].get<int32>() != image_asset.info.surface.height)
                    {
                        free(text_data);
                        throw std::runtime_error("Image mismatch: " + image_name);
                    }

                    // remove unused
                    j_meta.erase("app");
                    j_meta.erase("version");
                    j_meta.erase("smartupdate");
                    j_meta.erase("image");
                    j_meta.erase("size");

                    import.info.spritesheet.surface_id = table_id;
                    j_meta["image_table_id"] = table_id;
                    std::vector<uint8> msgpack = json::to_msgpack(j);

                    free(text_data);
                    import.data = malloc(msgpack.size());
                    if (!import.data)
                    {
                        throw std::runtime_error("failed memory allocation");
                    }

                    memcpy(import.data, msgpack.data(), msgpack.size());
                    import.info.size = msgpack.size();
                    import.enums = json::array();

                    {
                        /**********************************************************
                         *               Create enums for each frame
                         *********************************************************/

                        json j_enum = { { "name", import.name + "_spritesheet_frames" },
                                        { "values", json::array() } };

                        uint32 index = 0;
                        for (const auto& frame : j["frames"])
                        {
                            std::ostringstream ss;
                            ss << "frame_" << frame["filename"].get<std::string>();

                            // Persist the frame index if supplied.  This occurs for variable
                            // sized tilesets created from the 'Tiled' app and corresponds to
                            // the 'ID' field of the tileset.
                            //
                            // NOTE: It's been a while since implementing this functionality,
                            //       but I believe the reasoning is so the ID can be used
                            //       outside of the Tiled app when world building.  For
                            //       example, I've considered building a web interface to
                            //       correspond with the tilemap.  A sign defined in the
                            //       Tiled app could then have it's logic added outside the
                            //       app, like what it says, etc.
                            //
                            //       That said, I'm not sure why it'd be required at all,
                            //       but I'm choosing to leave it here for now and revisit.
                            if (frame.count("index"))
                            {
                                index = frame["index"].get<uint32>();
                            }

                            j_enum["values"].push_back({{ "n", ss.str() }, { "v", index++ }});
                        }

                        import.enums.push_back(j_enum);
                    }

                    if (j.count("animations"))
                    {
                        /**********************************************************
                         *               Create enums for each animation
                         *********************************************************/

                        json j_enum = { { "name", import.name + "_spritesheet_animations" },
                                        { "values", json::array() } };

                        uint32 index = 0;
                        const auto& j_animations = j["animations"];
                        for (const auto& j_animation : j_animations)
                        {
                            std::ostringstream ss;
                            ss << "animation_" << j_animation["name"].get<std::string>();

                            j_enum["values"].push_back({{ "n", ss.str() }, { "v", index++ }});
                        }

                        import.enums.push_back(j_enum);
                    }

                    if (j_meta.count("slices"))
                    {
                        /**********************************************************
                         *               Create enums for each slice
                         *********************************************************/

                        json j_enum = { { "name", import.name + "_spritesheet_slices" },
                                        { "values", json::array() } };

                        uint32 index = 0;
                        const auto& j_slices = j_meta["slices"];
                        for (const auto& j_slice : j_slices)
                        {
                            std::ostringstream ss;
                            ss << "slice_" << j_slice["name"].get<std::string>();

                            j_enum["values"].push_back({{ "n", ss.str() }, { "v", index++ }});
                        }

                        import.enums.push_back(j_enum);
                    }

                    std::cout << " SUCCESS"
                              << " surface_id=" << import.info.spritesheet.surface_id
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
        std::cout << "  Subdirectory " << SPRITESHEET_DIR << " not found\n";
    }

    return result;
}
