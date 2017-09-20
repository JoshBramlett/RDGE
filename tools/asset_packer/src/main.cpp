#include <rdge/core.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/assets/file_format.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>

#define TINYFILES_IMPL
#include <tinyheaders/tinyfiles.h>

#define STB_IMAGE_IMPLEMENTATION
#include <nothings/stb_image.h>

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
                  << std::endl;

        system_import_result totals;
        totals.success += surfaces.success;
        totals.failed += surfaces.failed;
        totals.skipped += surfaces.skipped;

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
    size_t size;
    void* data;

    rdge::asset_pack::asset_info info;
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
                import.size = file.size;
                import.data = stbi_load(file.path,
                                        &import.info.surface.width,
                                        &import.info.surface.height,
                                        &import.info.surface.format,
                                        0);
                if (import.data)
                {
                    std::string format_string = "unknown";
                    if (import.info.surface.format == STBI_rgb)
                    {
                        format_string = "RGB";
                    }
                    else if (import.info.surface.format == STBI_rgb_alpha)
                    {
                        format_string = "RGBA";
                    }

                    std::cout << " SUCCESS"
                              << " [" << import.info.surface.width
                              << "x" << import.info.surface.height << "]"
                              << " format=" << format_string
                              << " size=" << import.size << std::endl;

                    imported_assets.push_back(import);
                    import_results.surfaces.success++;

                    globals.running_count++;
                    globals.running_offset += import.size;
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

struct generated_header_file
{
    generated_header_file (const std::string file)
        : ofs(file, std::ofstream::out | std::ofstream::trunc)
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
            << "{\n"
            << "    " << globals.title << "_asset_invalid = 0,\n";

        ofs.flush();
    }

    ~generated_header_file (void) noexcept
    {
        ofs << "};\n";
        ofs.flush();

        ofs.close();
    }

    void add_enum (const imported_asset& asset)
    {
        ofs << "    " << globals.title << "_asset_";

        switch (asset.info.type)
        {
        case rdge::asset_pack::asset_type_surface:
            ofs << "surface";
            break;
        case rdge::asset_pack::asset_type_font:
            ofs << "font";
            break;
        case rdge::asset_pack::asset_type_spritesheet:
            ofs << "spritesheet";
            break;
        case rdge::asset_pack::asset_type_tilemap:
            ofs << "tilemap";
            break;
        case rdge::asset_pack::asset_type_sound:
            ofs << "sound";
            break;
        default:
            ofs << "unknown";
            break;
        }

        ofs << "_" << asset.name << " = " << asset.table_id << "," << std::endl;
    }

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

    import_results.print();

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
            gen_header.add_enum(import);
            import.info.offset += asset_table_size;
            fwrite(&import, sizeof(asset_info), 1, pack_file);
        }

        for (const auto& import : imported_assets)
        {
            fwrite(import.data, import.size, 1, pack_file);
        }

        fclose(pack_file);
    }
    else
    {
        std::cout << "ERROR: Couldn't open file" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
