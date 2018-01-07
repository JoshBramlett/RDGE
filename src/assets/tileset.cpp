#include <rdge/assets/tileset.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <exception>
#include <sstream>

namespace rdge {

using namespace rdge::math;
using json = nlohmann::json;

namespace {

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

void
ProcessTileset (const json& j, Tileset& tileset)
{
    JSON_VALIDATE_REQUIRED(j, tileheight, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tilewidth, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, spacing, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, margin, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, tilecount, is_number_unsigned);
    JSON_VALIDATE_REQUIRED(j, columns, is_number_unsigned);

    auto tile_count = j["tilecount"].get<size_t>();
    auto tile_width = j["tilewidth"].get<size_t>();
    auto tile_height = j["tileheight"].get<size_t>();

    tileset.cols = j["columns"].get<size_t>();
    tileset.rows = tile_count / tileset.cols;
    tileset.spacing = j["spacing"].get<size_t>();
    tileset.margin = j["margin"].get<size_t>();
    tileset.tile_size = math::vec2(static_cast<float>(tile_width),
                                   static_cast<float>(tile_height));
    tileset.tiles.reserve(tile_count);

    if (tileset.spacing != 0)
    {
        throw std::invalid_argument("Tile spacing not currently supported");
    }

    auto surface_size = tileset.surface.Size();
    for (size_t row = 0; row < tileset.rows; row++)
    {
        for (size_t col = 0; col < tileset.cols; col++)
        {
            auto x = tileset.margin + (col * tile_width);
            auto y = tileset.margin + (row * tile_height);

            float x1 = normalize(x, surface_size.w);
            float x2 = normalize(x + tile_width, surface_size.w);
            float y1 = normalize(y, surface_size.h);
            float y2 = normalize(y + tile_height, surface_size.h);

            tex_coords coords;
            coords.bottom_left  = vec2(x1, y1);
            coords.bottom_right = vec2(x2, y1);
            coords.top_left     = vec2(x1, y2);
            coords.top_right    = vec2(x2, y2);

            tileset.tiles.push_back(coords);
        }
    }
}

} // anonymous namespace

Tileset::Tileset (const char* filepath)
{
    try
    {
        auto rwops = rwops_base::from_file(filepath, "rt");
        auto sz = rwops.size();

        std::string file_data(sz + 1, '\0');
        rwops.read(file_data.data(), sizeof(char), sz);

        const auto j = json::parse(file_data);
        JSON_VALIDATE_REQUIRED(j, image, is_string);
        JSON_VALIDATE_REQUIRED(j, imageheight, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j, imagewidth, is_number_unsigned);

        this->surface = Surface(j["image"].get<std::string>());

        // sanity check
        if (this->surface.Width() != j["imagewidth"].get<size_t>() ||
            this->surface.Height() != j["imageheight"].get<size_t>())
        {
            throw std::invalid_argument("Surface size mismatch");
        }

        ProcessTileset(j, *this);
    }
    catch (const std::logic_error& ex)
    {
        RDGE_THROW(ex.what());
    }
}

Tileset::Tileset (const std::vector<uint8>& msgpack, PackFile& packfile)
{
    try
    {
        json j = json::from_msgpack(msgpack);
        JSON_VALIDATE_REQUIRED(j, image_table_id, is_number);
        JSON_VALIDATE_REQUIRED(j, imageheight, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j, imagewidth, is_number_unsigned);

        this->surface = packfile.GetSurface(j["image_table_id"].get<int32>());

        // sanity check
        if (this->surface.Width() != j["imagewidth"].get<size_t>() ||
            this->surface.Height() != j["imageheight"].get<size_t>())
        {
            throw std::invalid_argument("Surface size mismatch");
        }

        ProcessTileset(j, *this);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

} // namespace rdge
