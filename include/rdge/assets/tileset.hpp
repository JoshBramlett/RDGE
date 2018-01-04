//! \headerfile <rdge/assets/tileset.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/26/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Tileset
//! \brief Load tileset from a json config
//! \details Tileset represents the breakdown of the pixel data into individual
//!          tiles.  The tiles all have the same size, and unlike the regions
//!          of a sprite sheet, have no internal data stored for each tile.
//!          A \ref Tilemap will contain a map of Tileset indices for rendering
//!          a scene.
//!
//!          The json proprietary format is an expansion on the Tiled
//!          default format.
//!
//! \code{.json}
//! {
//!   "type": "tileset",
//!   "name": "overworld_bg",
//!   "tileheight": 16,
//!   "tilewidth": 16,
//!   "image": "../images/overworld_bg.png",
//!   "spacing": 0,
//!   "tilecount": 440,
//!   "imageheight": 354,
//!   "imagewidth": 322,
//!   "margin": 1,
//!   "columns": 20
//! }
//! \endcode
class Tileset
{
public:
    //! \brief Tileset ctor
    //! \details Loads and parses the json file.
    //! \param [in] filepath Path to the config file
    //! \throws rdge::Exception Unable to parse config
    explicit Tileset (const char* filepath);

    //! \brief Tileset ctor
    //! \details Loads and parses the packed json (used with \ref PackFile).
    //! \param [in] msgpack Packed json configuration
    //! \param [in] packfile \ref PackFile reference (to load dependencies)
    //! \throws rdge::Exception Unable to parse config
    //! \see http://msgpack.org/
    explicit Tileset (const std::vector<uint8>& msgpack, PackFile& pack);

    //!@{ Tileset default ctor/dtor
    Tileset (void) = default;
    ~Tileset (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    Tileset (const Tileset&) = delete;
    Tileset& operator= (const Tileset&) = delete;
    Tileset (Tileset&&) noexcept = default;
    Tileset& operator= (Tileset&&) noexcept = default;
    //!@}

public:
    size_t rows = 0;
    size_t cols = 0;
    size_t spacing = 0;
    size_t margin = 0;

    math::vec2 tile_size;
    std::vector<tex_coords> tiles;

    Surface surface; //!< Pixel data of the tileset
};

} // namespace rdge
