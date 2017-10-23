//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/assets/spritesheet_region.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Sprite;
class SpriteGroup;
class Texture;
class Animation;
struct region_data;
struct animation_data;
//!@}

//! \struct tilemap_data
//! \brief Contains the definition of a map of texture coordinates which will
//!        be rendered as a contiguous image.
struct tilemap_data
{
    static constexpr size_t MAX_LAYER_COUNT = 4; //!< Maximum amount of layers

    //! \brief Denotes that a tile should not be rendered
    //! \details Common with multi-layered tile maps, where higher layers could
    //!          have sparse tile placement
    static constexpr int32 INVALID_TILE = -1;

    tilemap_data (void) = default;
    ~tilemap_data (void) noexcept;

    //!@{ Copy and move enabled
    tilemap_data (const tilemap_data&);
    tilemap_data& operator= (const tilemap_data&);
    tilemap_data (tilemap_data&&) noexcept;
    tilemap_data& operator= (tilemap_data&&) noexcept;
    //!@}

    //! \struct tile
    //! \brief Individual tile in the map
    struct tile
    {
        int32 index = INVALID_TILE; //!< Layer index, or \ref INVALID_TILE
        math::uivec2 location;      //!< Coordinates of the tile in the map
        tex_coords coords;          //!< UV data for the texture
    };

    //! \struct layer
    //! \brief Layer of tiles, which should be rendered bottom to top
    struct layer
    {
        tile* tiles = nullptr; //!< Array of tile_count tiles
    };

    layer layers[MAX_LAYER_COUNT];
    size_t layer_count = 0;

    size_t tile_count = 0; //!< Number of tiles per layer
    size_t tile_pitch = 0; //!< Number of tiles in a row
    math::vec2 tile_size;  //!< Tile dimensions
};

//! \class SpriteSheet
//! \brief Load sprite sheet from a json config
//! \details SpriteSheet (aka TextureAtlas) represents the definition of how
//!          pixel data is broken down to individual sprites.  The definition
//!          is parsed from an external json resource, and includes support
//!          for defining the texture regions (with optional support to define
//!          animations), and tile maps.
//!
//!          The root level field "type" is required to define how the json
//!          resource is formatted.  Types include:
//!            - "spritesheet"
//!            - "tilemap"
//!
//!          A "spritesheet" resource defines the regions and has optional
//!          support for defining animations.
//!
//! \code{.json}
//! {
//!     "image_path": "textures/image.png",
//!     "type": "spritesheet",
//!     "margin": 1,
//!     "regions": [ {
//!         "name": "region_name",
//!         "x": 0,
//!         "y": 0,
//!         "width": 32,
//!         "height": 32,
//!         "origin": [ 16, 16 ]
//!     } ],
//!     "animations": [ {
//!         "name": "animation_name",
//!         "mode": "normal",
//!         "interval": 100,
//!         "frames": [ {
//!             "name": "part_name",
//!             "flip": "horizontal"
//!         } ]
//!     } ]
//! }
//! \endcode
//!
//!          A "tilemap" resource generates the region data from the tile
//!          definition.  The mapping is also parsed and cached.
//!
//! \code{.json}
//! {
//!     "image_path": "textures/image.png",
//!     "type": "tilemap",
//!     "margin": 1,
//!     "tileswide": 30,
//!     "tileshigh": 30,
//!     "tilewidth": 16,
//!     "tileheight": 16,
//!     "layers": [ {
//!         "name": "Layer 0",
//!         "number": 0,
//!         "tiles": [ {
//!             "x": 29,
//!             "y": 29,
//!             "flipX": false,
//!             "rot": 0,
//!             "index": 899,
//!             "tile": 29
//!         } ],
//!     } ]
//! }
//! \endcode
//!
//! \note Type checking is done on all fields, and logical checks are performed
//!       where applicable (e.g. Region cannot exceed surface size), but no
//!       checking is done to ensure parts are unique or do not overlap.
class SpriteSheet
{
public:
    //! \brief SpriteSheet default ctor
    SpriteSheet (void) = default;

    //! \brief SpriteSheet ctor
    //! \details Loads and parses the json file.
    //! \param [in] filepath Path to the config file
    //! \throws rdge::Exception Unable to parse config
    explicit SpriteSheet (const char* filepath);

    //! \brief SpriteSheet ctor
    //! \details Loads and parses the packed json (used with \ref PackFile)
    //! \param [in] msgpack Packed json data
    //! \param [in] surface Associated surface
    //! \throws rdge::Exception Unable to parse config
    //! \see http://msgpack.org/
    explicit SpriteSheet (const std::vector<uint8>& msgpack, Surface surface);

    //! \brief SpriteSheet dtor
    ~SpriteSheet (void) noexcept;

    //!@{ Non-copyable, move enabled
    SpriteSheet (const SpriteSheet&) = delete;
    SpriteSheet& operator= (const SpriteSheet&) = delete;
    SpriteSheet (SpriteSheet&&) noexcept;
    SpriteSheet& operator= (SpriteSheet&&) noexcept;
    //!@}

    //! \brief SpriteSheet Subscript Operator
    //! \details Retrieves texture coordinates by name
    //! \param [in] name Name of the element
    //! \returns Associated tex_coords
    //! \throws rdge::Exception Lookup failed
    const spritesheet_region& operator[] (const std::string& name) const;

    //!@{
    //! \brief Retrive an \ref Animation
    //! \throws rdge::Exception Lookup failed
    const Animation& GetAnimation (const std::string& name) const;
    const Animation& GetAnimation (int32 animation_id) const;
    //!@}



    // TODO
    // - Add getter for the spritesheet_region
    // - Support scaling?
    // - Check for duplicate keys during import?

    // TODO Remove
    std::unique_ptr<Sprite> CreateSprite (const std::string& name,
                                          const math::vec3& pos) const;

    // TODO Remove
    std::unique_ptr<SpriteGroup> CreateSpriteChain (const std::string& name,
                                                    const math::vec3&  pos,
                                                    const math::vec2&  to_fill) const;

public:
    Surface surface; //!< Pixel data of the sprite sheet

    // TODO Consider remove
    std::shared_ptr<Texture> texture; //!< Texture generated from the surface

    //!@{ Region container
    region_data* regions = nullptr;
    size_t       region_count = 0;
    //!@}

    //!@{ Animation container
    animation_data* animations = nullptr;
    size_t          animation_count = 0;
    //!@}

    //!@{ Tilemap container
    tilemap_data tilemap;
    //!@}
};

} // namespace rdge
