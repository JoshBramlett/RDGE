//! \headerfile <rdge/assets/tilemap/tilemap.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/22/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>

//!@{ Forward declarations
//namespace nlohmann { class json; }
// TODO Add forward declaration
// https://github.com/nlohmann/json/issues/314
#include <rdge/util/json.hpp>
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace tilemap {

//! \enum Orientation
//! \brief Tilemap rendering orientation
enum class Orientation
{
    INVALID = -1,
    ORTHOGONAL,
    ISOMETRIC,
    STAGGERED,
    HEXAGONAL
};

//! \class Tilemap
//! \brief Generic tilemap layer
//! \details Tilemap represents the collection of all the entities that make up
//!          the scene.
//!
//!          The proprietary json format expands on the Tiled default format.
//!
//! \code{.json}
//! {
//!   "type": "map",
//!   "backgroundcolor": "#656667",
//!   "height": 4,
//!   "orientation": "orthogonal",
//!   "renderorder": "right-down",
//!   "tileheight": 32,
//!   "tilewidth": 32,
//!   "version": 1,
//!   "width": 4
//!   "layers": [ ... ],
//!   "tilesets": [ ... ],
//!   "properties": [ ... ]
//! }
//! \endcode
class Tilemap
{
public:
    //! \brief Tilemap ctor
    //! \param [in] j json formatted layer
    //! \throws rdge::Exception Parsing failed
    Tilemap (const nlohmann::json& j);

    //!@{ Tilemap default ctor/dtor
    Tilemap (void) = default;
    ~Tilemap (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    Tilemap (const Tilemap&) = delete;
    Tilemap& operator= (const Tilemap&) = delete;
    Tilemap (Tilemap&&) noexcept = default;
    Tilemap& operator= (Tilemap&&) noexcept = default;
    //!@}

public:

    //!@{ Rendering properties
    color background = color::BLACK;
    Orientation orientation = Orientation::INVALID;
    TileRenderOrder render_order = TileRenderOrder::INVALID;
    //!@}

    //!@{ Grid properties
    math::vec2 cell_size; //!< Map grid cell size
    size_t rows = 0;      //!< Number of rows in the grid
    size_t cols = 0;      //!< Number of columns in the grid
    //!@}

    struct sheet_info
    {
        int32 first_gid;             //!< First global tile id in the sheet
        int32 table_id;              //!< Sheet table id in the \ref PackFile
        asset_pack::asset_type type; //!< Sheet type (spritesheet or tileset)
    }

    std::vector<Layer> layers;      //!< List of Tilemap layers
    std::vector<sheet_info> sheets; //!< List of Tilesets/Spritesheets dependencies
    PropertyCollection properties;  //!< Custom variable type property collection
};

//! \brief Orientation stream output operator
std::ostream& operator<< (std::ostream&, Orientation);

} // namespace tilemap

//!@{ Orientation string conversions
bool try_parse (const std::string&, tilemap::Orientation&);
std::string to_string (tilemap::Orientation);
//!@}

} // namespace rdge
