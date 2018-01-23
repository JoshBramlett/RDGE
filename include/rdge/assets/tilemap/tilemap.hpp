//! \headerfile <rdge/assets/tilemap/tilemap.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/22/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>

#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/tileset.hpp>
#include <rdge/assets/spritesheet.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class PackFile;
//!@}

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
//!   "orientation": "orthogonal",
//!   "version": 1,
//!   "grid": {
//!     "cells": {
//!       "width": 16,
//!       "height": 16
//!     },
//!     "height": 80,
//!     "width": 96,
//!     "renderorder": "right-down",
//!     "y": -16,
//!     "x": -16,
//!     "chunks": {
//!       "width": 16,
//!       "height": 16
//!     }
//!   },
//!   "layers": [ ... ],
//!   "tilesets": [ ... ],
//!   "properties": [ ... ]
//! }
//! \endcode
class Tilemap
{
public:
    //! \brief Tilemap ctor
    //! \details Loads and parses the packed json (used with \ref PackFile).
    //! \param [in] msgpack Packed json configuration
    //! \param [in] packfile \ref PackFile reference (to load dependencies)
    //! \throws rdge::Exception Parsing failed
    //! \see http://msgpack.org/
    Tilemap (const std::vector<uint8>& msgpack, PackFile& packfile);

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

    //TileLayer CreateTileLayer (int32 layer_id, float scale);
public:

    //!@{ Rendering properties
    color background = color::BLACK;
    Orientation orientation = Orientation::INVALID;
    //!@}

    struct sheet_info
    {
        int32 first_gid;             //!< First global tile id in the sheet
        int32 table_id;              //!< Sheet table id in the \ref PackFile
        asset_pack::asset_type type; //!< Sheet type (spritesheet or tileset)

        shared_asset<Tileset> tileset;
        shared_asset<SpriteSheet> spritesheet;
    };

    tilemap_grid grid;              //!< Grid for the Tilemap
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
