//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/assets/spritesheet_region.hpp>

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
struct tile_data;
//!@}

//! \struct tilemap_tile
//! \brief Container for a tile in the tilemap
struct tilemap_tile
{
    math::uivec2       location; //!< Coordinates of the tile in the map
    spritesheet_region region;   //!< Region data, which may be flipped/rotated
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

    //! \brief Retrive a tile from the tilemap
    tilemap_tile GetTile (size_t index);



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
    tile_data* tiles = nullptr;
    size_t     tile_pitch = 0;
    size_t     tile_count = 0;
    //!@}
};

} // namespace rdge
