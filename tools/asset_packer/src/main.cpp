#include <rdge/core.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/assets/asset_pack.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdexcept>

#define TINYFILES_IMPL
#include <tinyheaders/tinyfiles.h>

#define STB_IMAGE_IMPLEMENTATION
#include <nothings/stb_image.h>

#include <nlohmann/json.hpp>

#define IMAGE_DIR "images"
#define FONT_DIR "fonts"
#define SPRITESHEET_DIR "spritesheets"
#define TILEMAP_DIR "tilemaps"


// Asset packer
//
// Command line utility that builds a packed asset file and corresponding
// header file (.hpp) that contains an enum that maps to each imported asset.
//
// To run the packer expects a parent directory that contains sub-directories
// containing all the different asset types.
//
// parent_directory/
//     images/
//     fonts/
//     spritesheets/
//     tilemaps/
//
// The packer will iterate through each child directory and import every
// valid file.
//
// Usage:
//  Arg 1) Override the parent directory
//  Arg 2) Override the title (used in file names and enum values)

using json = nlohmann::json;
using namespace rdge;

struct global_properties
{
    std::string parent_dir = "./"; // directory where files are found
    std::string title = "rdge";   // name of the pack file and header
    uint32 running_count = 0;     // running asset id
    uint64 running_offset = 0;    // running asset offset
} globals;

struct total_import_result
{
    total_import_result (void) = default;

    struct system_import_result
    {
        size_t success = 0;
        size_t failed = 0;
        size_t skipped = 0;
    };

    system_import_result surfaces;
    system_import_result spritesheets;
    system_import_result tilemaps;

    void print (void)
    {
        std::cout << "\nImport Summary\n\n"
                  << "Image:       "
                  << "  success: " << std::right << std::setw(3) << surfaces.success
                  << "  failed:  " << std::right << std::setw(3) << surfaces.failed
                  << "  skipped: " << std::right << std::setw(3) << surfaces.skipped
                  << std::endl
                  << "Spritesheet: "
                  << "  success: " << std::right << std::setw(3) << spritesheets.success
                  << "  failed:  " << std::right << std::setw(3) << spritesheets.failed
                  << "  skipped: " << std::right << std::setw(3) << spritesheets.skipped
                  << std::endl;

        system_import_result totals;
        totals.success = surfaces.success + spritesheets.success;
        totals.failed = surfaces.failed + spritesheets.failed;
        totals.skipped = surfaces.skipped + spritesheets.skipped;

        std::cout << "-------------------------------------------------------\n"
                  << "Total:       "
                  << "  success: " << std::right << std::setw(3) << totals.success
                  << "  failed:  " << std::right << std::setw(3) << totals.failed
                  << "  skipped: " << std::right << std::setw(3) << totals.skipped
                  << std::endl;
    }

} import_results;

struct imported_asset
{
    std::string name;
    uint32 table_id;
    void* data;

    rdge::asset_pack::asset_info info;

    std::vector<std::string> enums; // string of the entire enum to be written
};

std::vector<imported_asset> imported_assets;


bool
IsImageFileSupported (const std::string& file)
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

void
ImportImages (void)
{
    std::string path = globals.parent_dir + IMAGE_DIR;
    std::cout << "ImportImages from " << path << std::endl;

	tfDIR dir;
	if (tfDirOpen(&dir, path.c_str()))
    {
        while (dir.has_next)
        {
            //struct tfFILE
            //{
                //char path[ TF_MAX_PATH ];
                //char name[ TF_MAX_FILENAME ];
                //char ext[ TF_MAX_EXT ];
                //int is_dir;
                //int is_reg;
                //int size;
                //struct stat info;
            //};

            tfFILE file;
            tfReadFile(&dir, &file);

            if (file.is_dir)
            {
                tfDirNext(&dir);
                continue;
            }

            if (file.is_reg && IsImageFileSupported(file.name))
            {
                std::cout << "  Processing [" << file.name << "]";

                imported_asset import;
                import.info.type = rdge::asset_pack::asset_type_surface;
                import.info.offset = globals.running_offset;
                import.name = rdge::remove_extension(file.name);
                import.table_id = globals.running_count;
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

                    imported_assets.push_back(import);
                    import_results.surfaces.success++;

                    globals.running_count++;
                    globals.running_offset += import.info.size;
                }
                else
                {
                    std::cout << " FAILED stbi_load"
                              << " reason=" << stbi_failure_reason() << std::endl;
                    import_results.surfaces.failed++;
                }
            }
            else
            {
                std::cout << "  Skipping [" << file.name << "] unsupported type\n";
                import_results.surfaces.skipped++;
            }

            tfDirNext(&dir);
        }

        tfDirClose(&dir);
    }
    else
    {
        std::cout << "  Subdirectory " << IMAGE_DIR << " not found" << std::endl;
    }
}

void
ImportSpritesheets (void)
{
    std::string path = globals.parent_dir + SPRITESHEET_DIR;
    std::cout << "ImportSpritesheets from " << path << std::endl;

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

            if (file.is_reg && rdge::ends_with(file.name, "json"))
            {
                std::cout << "  Processing [" << file.name << "]";

                imported_asset import;
                import.info.type = rdge::asset_pack::asset_type_spritesheet;
                import.info.offset = globals.running_offset;
                import.name = rdge::remove_extension(file.name);
                import.table_id = globals.running_count;

                try
                {
                    auto rwops = rwops_base::from_file(file.path, "rt");
                    auto text_size = rwops.size();
                    char* text_data = (char*)calloc(text_size + 1, sizeof(char));
                    if (!text_data)
                    {
                        free(text_data);
                        throw std::runtime_error("failed memory allocation");
                    }

                    rwops.read(text_data, text_size);

                    const auto j = json::parse(text_data);
                    auto sp = j["image_path"].get<std::string>();
                    auto surface_name = rdge::remove_path(rdge::remove_extension(sp));
                    int index = -1;
                    for (size_t i = 0; i < imported_assets.size(); i++)
                    {
                        const auto& test = imported_assets[i];
                        if (test.info.type == rdge::asset_pack::asset_type_surface &&
                            test.name == surface_name)
                        {
                            index = static_cast<int>(i);
                            break;
                        }
                    }

                    if (index < 0)
                    {
                        free(text_data);
                        throw std::runtime_error("spritesheet cannot map to surface");
                    }

                    import.info.spritesheet.surface_id = index;
                    std::vector<uint8> msgpack = json::to_msgpack(j);

                    free(text_data);
                    import.data = malloc(msgpack.size());
                    if (!import.data)
                    {
                        throw std::runtime_error("failed memory allocation");
                    }

                    memcpy(import.data, msgpack.data(), msgpack.size());
                    import.info.size = msgpack.size();

                    if (j.count("texture_parts") > 0)
                    {
                        std::ostringstream ss;
                        ss << "enum " << import.name << "_spritesheet_regions\n"
                           << "{\n";

                        const auto& regions = j["texture_parts"];
                        int v = 0;
                        for (const auto& region : regions)
                        {
                            auto n = region["name"].get<std::string>();
                            ss << "    " << import.name << "_region_" << n << " = " << v++ << ",\n";
                        }

                        ss << "};";

                        import.enums.emplace_back(ss.str());
                    }

                    if (j.count("animations") > 0)
                    {
                        std::ostringstream ss;
                        ss << "enum " << import.name << "_spritesheet_animations\n"
                           << "{\n";

                        const auto& animations = j["animations"];
                        int v = 0;
                        for (const auto& animation : animations)
                        {
                            auto n = animation["name"].get<std::string>();
                            ss << "    " << import.name << "_animation_" << n << " = " << v++ << ",\n";
                        }

                        ss << "};";

                        import.enums.emplace_back(ss.str());
                    }

                    std::cout << " SUCCESS"
                              << " surface_id=" << import.info.spritesheet.surface_id
                              << " file_size=" << file.size
                              << " import_size=" << import.info.size << std::endl;

                    imported_assets.push_back(import);
                    import_results.spritesheets.success++;

                    globals.running_count++;
                    globals.running_offset += import.info.size;
                }
                catch (const std::exception& ex)
                {
                    std::cout << " FAILED on exception"
                              << " reason=" << ex.what() << std::endl;
                    import_results.surfaces.failed++;
                }
                catch (...)
                {
                    std::cout << " FAILED on unknown exception" << std::endl;
                    import_results.surfaces.failed++;
                }
            }
            else
            {
                std::cout << "  Skipping [" << file.name << "] unsupported type\n";
                import_results.surfaces.skipped++;
            }

            tfDirNext(&dir);
        }

        tfDirClose(&dir);
    }
    else
    {
        std::cout << "  Subdirectory " << IMAGE_DIR << " not found" << std::endl;
    }
}

struct generated_header_file
{
    generated_header_file (const std::string file)
        : ofs(file, std::ofstream::out | std::ofstream::trunc)
    {
    }

    ~generated_header_file (void) noexcept
    {
        ofs.close();
    }

    void write (void)
    {
        std::time_t now = std::time(NULL);
        std::tm* t = std::localtime(&now);
        char buffer[11];
        std::strftime(buffer, 11, "%m/%d/%Y", t);

        ofs << "/************************************************************/\n"
            << "/**              File generated by asset_packer            **/\n"
            << "/************************************************************/\n"
            << "\n"
            << "// Created: " << buffer << "\n"
            << "// Version: " << RDGE_ASSET_PACK_VERSION << "\n"
            << "\n"
            << "#pragma once\n"
            << "\n"
            << "enum " << globals.title << "_asset_pack_table\n"
            << "{\n";

        for (const auto& e : asset_pack_table)
        {
            ofs << e;
        }

        ofs << "};\n";

        for (const auto& e : asset_enums)
        {
            ofs << "\n" << e << "\n";
        }

        ofs.flush();
    }

    void add_enum_value (const imported_asset& asset)
    {
        std::ostringstream oss;
        oss << "    " << globals.title << "_asset_";

        switch (asset.info.type)
        {
        case rdge::asset_pack::asset_type_surface:
            oss << "surface";
            break;
        case rdge::asset_pack::asset_type_font:
            oss << "font";
            break;
        case rdge::asset_pack::asset_type_spritesheet:
            oss << "spritesheet";
            break;
        case rdge::asset_pack::asset_type_tilemap:
            oss << "tilemap";
            break;
        case rdge::asset_pack::asset_type_sound:
            oss << "sound";
            break;
        default:
            oss << "unknown";
            break;
        }

        oss << "_" << asset.name << " = " << asset.table_id << "," << std::endl;
        asset_pack_table.emplace_back(oss.str());
    }

    std::vector<std::string> asset_pack_table;
    std::vector<std::string> asset_enums;
    std::ofstream ofs;
};

int
main (int argc, char** argv)
{
    if (argc > 1)
    {
        globals.parent_dir = argv[1];
        if (!rdge::ends_with(globals.parent_dir, "/"))
        {
            globals.parent_dir += '/';
        }
    }

    if (argc > 2)
    {
        globals.title = argv[2];
    }

    ImportImages();
    ImportSpritesheets();

    import_results.print();

    std::cout << "\nContinue and write files? (y/n)\n";
    char cont;
    while (std::cin >> cont)
    {
        if (cont == 'y')
        {
            break;
        }
        else if (cont == 'n')
        {
            std::cout << "\nAborted\n";
            return EXIT_SUCCESS;
        }
    }

    std::string pack_file_name = globals.title + ".data";
    std::string pack_header_name = globals.title + ".hpp";

    FILE* pack_file = fopen(pack_file_name.c_str(), "wb");
    generated_header_file gen_header(pack_header_name);
    if (pack_file && gen_header.ofs.is_open())
    {
        using namespace rdge::asset_pack;

        header h = {};
        h.magic_value = RDGE_MAGIC_VALUE;
        h.version = RDGE_ASSET_PACK_VERSION;
        h.asset_count = globals.running_count;
        h.assets = sizeof(header);
        fwrite(&h, sizeof(header), 1, pack_file);

        uint32 asset_table_size = h.asset_count * sizeof(asset_info);
        for (auto& import : imported_assets)
        {
            gen_header.add_enum_value(import);
            for (const auto& e : import.enums)
            {
                gen_header.asset_enums.push_back(e);
            }

            import.info.offset += sizeof(header) + asset_table_size;
            fwrite(&import.info, sizeof(asset_info), 1, pack_file);
        }

        for (const auto& import : imported_assets)
        {
            fwrite(import.data, import.info.size, 1, pack_file);
            free(import.data);
        }

        gen_header.write();
        fclose(pack_file);
    }
    else
    {
        std::cout << "ERROR: Couldn't open file" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\nFinished\n";
    return EXIT_SUCCESS;
}
