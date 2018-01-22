//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/assets/spritesheet_region.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/graphics/animation.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class PackFile;
//!@}

//! \struct region_data
//! \brief Expanded read-only \ref spritesheet_region container
//! \details Provides further details about the imported region
struct region_data
{
    std::string        name;  //!< Name as specified by import
    spritesheet_region value; //!< Core region data, including size, uv coords, etc.

    //! \brief Collection of tile objects
    //! \details Used with object sheets, tile objects are shapes for creating the
    //!          collision data for the provided region.
    std::vector<tilemap::Object> objects;
};

//! \struct animation_data
//! \brief Expanded read-only \ref Animation container
//! \details Provides further details about the imported animation
struct animation_data
{
    std::string name;  //!< Name as specified by import
    Animation   value; //!< Imported animation
};

//! \class SpriteSheet
//! \brief Load sprite sheet from a json config
//! \details SpriteSheet (aka TextureAtlas) represents the definition of how
//!          pixel data is broken down to individual sprites.  The definition
//!          is parsed from an external json resource, and includes support
//!          for defining the texture regions, and optional support to define
//!          animations whose frames are regions of the sprite sheet.
//!
//!          The sprite sheet may also be imported as an "object sheet", where
//!          the region data has additional properties defining the collision
//!          data.  These are known as "tile objects", and are intended to provide
//!          a full entity definition within a scene.
//!
//!          The json proprietary format is an expansion on the TexturePacker
//!          default format.  The differences include the animation array, and
//!          support for object sheets.  Object sheets supply their own indices
//!          as well as \ref tilemap::Object configurations for each region.
//!
//! \code{.json}
//! {
//!   "frames": [ {
//!     "index": 0,
//!     "frame": {
//!       "y": 2,
//!       "x": 2,
//!       "w": 75,
//!       "h": 94
//!     },
//!     "rotated": false,
//!     "filename": "tree_01",
//!     "trimmed": true,
//!     "objects": [ ... ],
//!     "pivot": {
//!       "y": 0.5,
//!       "x": 0.5
//!     },
//!     "sourceSize": {
//!       "h": 96,
//!       "w": 80
//!     },
//!     "spriteSourceSize": {
//!       "y": 2,
//!       "x": 2,
//!       "w": 75,
//!       "h": 94
//!     }
//!   } ],
//!   "animations": [ {
//!     "name": "animation_name",
//!     "mode": "normal",
//!     "interval": 100,
//!     "frames": [ {
//!       "name": "part_name",
//!       "flip": "horizontal"
//!     } ]
//!   } ]
//! }
//! \endcode
//!
//! \note Type checking is done on all fields, and logical checks are performed
//!       where applicable (e.g. Region cannot exceed surface size), but no
//!       checking is done to ensure parts are unique or do not overlap.
class SpriteSheet
{
public:
    //! \brief SpriteSheet ctor
    //! \details Loads and parses the json file.
    //! \param [in] filepath Path to the config file
    //! \throws rdge::Exception Unable to parse config
    explicit SpriteSheet (const char* filepath);

    //! \brief SpriteSheet ctor
    //! \details Loads and parses the packed json (used with \ref PackFile).
    //! \param [in] msgpack Packed json configuration
    //! \param [in] packfile \ref PackFile reference (to load dependencies)
    //! \throws rdge::Exception Unable to parse config
    //! \see http://msgpack.org/
    explicit SpriteSheet (const std::vector<uint8>& msgpack, PackFile& pack);

    //!@{ SpriteSheet default ctor/dtor
    SpriteSheet (void) = default;
    ~SpriteSheet (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    SpriteSheet (const SpriteSheet&) = delete;
    SpriteSheet& operator= (const SpriteSheet&) = delete;
    SpriteSheet (SpriteSheet&&) noexcept = default;
    SpriteSheet& operator= (SpriteSheet&&) noexcept = default;
    //!@}

    //! \brief SpriteSheet Subscript Operator
    //! \details Retrieves texture coordinates by name
    //! \param [in] name Name of the element
    //! \returns Associated region
    //! \throws rdge::Exception Lookup failed
    const spritesheet_region& operator[] (const std::string& name) const;

    //!@{
    //! \brief Retrive an \ref Animation
    //! \throws rdge::Exception Lookup failed
    Animation GetAnimation (const std::string& name, float scale = 1.f) const;
    Animation GetAnimation (int32 animation_id, float scale = 1.f) const;
    //!@}

public:
    std::vector<region_data> regions;       //!< Spritesheet region list
    std::vector<animation_data> animations; //!< Animation definition list

    shared_asset<Surface> surface; //!< Pixel data of the sprite sheet
};

} // namespace rdge
