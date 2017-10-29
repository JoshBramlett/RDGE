#include <rdge/util.hpp>
#include <rdge/assets.hpp>

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;
using namespace rdge;

void
print_usage (void)
{
    std::cout << "Usage:\n"
              << "pyxel2tiled [source]\n"
              << std::endl;
}

tiled::tilemap
convert (const pyxel_edit::tilemap& source)
{
    tiled::tilemap result;
    result.width = source.tileswide;
    result.height = source.tileshigh;
    result.tilewidth = source.tilewidth;
    result.tileheight = source.tileheight;

    size_t layer_count = source.layers.size();
    result.layers = std::vector<tiled::layer>(layer_count);
    for (size_t i = 0; i < layer_count; i++)
    {
        const auto& sl = source.layers[i];

        auto& rl = result.layers[i];
        rl.name = sl.name;
        rl.width = source.tileswide;
        rl.height = source.tileshigh;

        rl.data.reserve(sl.tiles.size());
        for (const auto& st : sl.tiles)
        {
            rl.data.push_back(static_cast<uint32>(st.tile + 1));
        }
    }

    result.tilesets = std::vector<tiled::tileset>(1);
    auto& rts = result.tilesets[0];

#if 1
    // Option A) External tileset
    rts.type = rdge::tiled::tileset::tiled_tileset_type_external;
    rts.source = "UNSET";
#else
    // Option B) Embedded tileset
    //
    // Unused b/c Pyxel Edit export is garbage and no info about the tileset is
    // provided in the json file.  To use embedded the following fields must
    // be manually set after conversion is ran.
    //
    // columns
    // tilecount
    // spacing
    // margin
    //
    // image
    // imagewidth
    // imageheight
    rts.type = rdge::tiled::tileset::tiled_tileset_type_embedded;
    rts.tilewidth = source.tilewidth;
    rts.tileheight = source.tileheight;
#endif

    return result;
}

void
pretty_hack (std::string& pretty)
{
    // hack to make the tiled layers.data on a single line.
    //
    // nlohmann::json does not currently support conditional formatting
    // https://github.com/nlohmann/json/issues/229

    auto n = pretty.find("\"data\":");
    while (n != std::string::npos)
    {
        auto start = pretty.find('[', n);
        auto end = pretty.find(']', start);

        if (start != std::string::npos && end != std::string::npos)
        {
            auto iter_start = pretty.begin() + start;
            auto iter_end = pretty.begin() + end;

            pretty.erase(std::remove_if(iter_start,
                                        iter_end,
                                        [](unsigned char x){return std::isspace(x);}),
                                        iter_end);

            n = pretty.find("\"data\":", start);
        }
        else
        {
            break;
        }
    }
}

int
main (int argc, char** argv)
{
    if (argc == 1)
    {
        print_usage();
        return EXIT_SUCCESS;
    }

    std::string source_file = argv[1];
    if (!rdge::ends_with(source_file, "json"))
    {
        std::cout << "ERROR: Invalid file format.  Only json files are supported\n";
        return EXIT_FAILURE;
    }

    std::string dest_file = rdge::remove_extension(source_file);
    dest_file += ".tiled.json";

    try
    {
        auto in = rwops_base::from_file(source_file.c_str(), "rt");
        auto text_size = in.size();
        char* text_data = (char*)calloc(text_size + 1, sizeof(char));
        if (!text_data)
        {
            throw std::runtime_error("failed memory allocation");
        }

        in.read(text_data, text_size);
        pyxel_edit::tilemap source_map = json::parse(text_data);

        in.close();
        free(text_data);

        nlohmann::json json_out = convert(source_map);
        std::string pretty = json_out.dump(4);
        pretty_hack(pretty);

        auto out = rwops_base::from_file(dest_file.c_str(), "wt");
        out.write(pretty.data(), pretty.length());

        if (out.close() != 0)
        {
            throw std::runtime_error("failed to close output stream");
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "ERROR: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Exported successfully\n"
              << dest_file << "\n\n";

    std::cout << "|================ IMPORTANT ================|\n"
              << "| Make sure tileset data is manually filled |\n"
              << "|===========================================|\n"
              << "| \"image\"                                   |\n"
              << "| \"imagewidth\"                              |\n"
              << "| \"imageheight\"                             |\n"
              << "| \"columns\"                                 |\n"
              << "| \"tilecount\"                               |\n"
              << "| \"spacing\"                                 |\n"
              << "| \"margin\"                                  |\n"
              << "|===========================================|\n\n";

    return EXIT_SUCCESS;
}
