//! \headerfile <rdge/assets/tileset.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/26/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class PackFile;
//!@}

//! \class Tileset
//! \brief Image asset that stores fixed size regions
//! \details Tileset represents the breakdown of the pixel data into individual
//!          tiles.  Unlike the regions of a \ref Spritesheet the tiles all have
//!          the same size.  A \ref Tilemap will contain a map of Tileset indices
//!          for rendering scene.  Optional tile animations may be mapped to a
//!          tile which includes the frame uvs and duration.
//!
//!          The proprietary json format expands on the Tiled default format.
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
//!   "columns": 20,
//!   "tiles": [{
//!     "id": 1,
//!     "animation": [{
//!       "tileid": 1,
//!       "duration": 500
//!     }]
//!   }]
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
    explicit Tileset (const std::vector<uint8>& msgpack, PackFile& packfile);

    //!@{ Tileset default ctor/dtor
    Tileset (void) = default;
    ~Tileset (void) noexcept;
    //!@}

    //!@{ Non-copyable, move enabled
    Tileset (const Tileset&) = delete;
    Tileset& operator= (const Tileset&) = delete;
    Tileset (Tileset&&) noexcept;
    Tileset& operator= (Tileset&&) noexcept;
    //!@}

public:
    //! \struct tile_frame
    //! \brief Frame for a tile animation
    struct tile_frame
    {
        uint32 tile_id;  //!< Tile Id of the frame
        uint32 duration; //!< Frame duration in milliseconds
    };

    //! \struct tile_animation
    //! \brief Collection of frames that comprise the animation
    struct tile_animation
    {
        tile_frame* frames;
        size_t frame_count;
    };

    //! \struct tile_data
    //! \brief Raw makeup of the \ref Tileset.
    //! \details Each tile has it's own unique uv, but it can also contain other
    //!          data such as the animation the tile points to.  Note that if the
    //!          frame points to an animation, it's no guarantee the uv of the
    //!          tile will be included as a frame in the animation.  It's up to
    //!          the consumer what functionality they want to utilize.
    struct tile_data
    {
        tex_coords uv;          //!< Unique coordindates of the tile
        int32 animation_index;  //!< Optional index to an animation (-1 if unmapped)
    };

public:
    math::vec2 tile_size;
    size_t rows = 0;
    size_t cols = 0;
    size_t spacing = 0;
    size_t margin = 0;

    tile_data* tiles = nullptr;
    size_t tile_count = 0;
    tile_animation* animations = nullptr;
    size_t animation_count = 0;
    tile_frame* frames = nullptr;
    size_t frame_count = 0;

    shared_asset<Surface> surface; //!< Pixel data of the tileset
};

} // namespace rdge
