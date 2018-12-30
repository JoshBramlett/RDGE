//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/graphics/animation.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class PackFile;
//!@}

//! \struct spritesheet_region
//! \brief Represents an individual section of the \ref SpriteSheet
//! \details Container includes the data the client can use for rendering and
//!          commonly represents a sprite texture or animation frame.
//! \note The size and origin values may be modified from the config to
//!       accommodate the scale multiplication.
struct spritesheet_region
{
    //! \brief Surface clipping rectangle
    screen_rect clip;

    //! \brief Normalized texture coordinates
    tex_coords coords;

    //! \brief Original size of the asset (in pixels)
    math::vec2 size;

    //! \brief Trimmed margin from the original size (in pixels)
    //! \details Margin is the pixels trimmed from the left (x-axis) and
    //!          the bottom (y-axis) of the sprite.
    math::vec2 sprite_offset;

    //! \brief Actual size after trimming (in pixels)
    math::vec2 sprite_size;

    //! \brief Pivot origin normalized to the sprite size
    //! \note Defaults to the centroid.
    math::vec2 origin;

    bool is_rotated; //!< Whether the TexturePacker rotated the region 90 clockwise

    //!@{ Basic spritesheet_region transforms
    void flip (TexCoordsFlip) noexcept;
    void rotate (TexCoordsRotation) noexcept;
    void scale (float) noexcept;
    //!@}
};

//! \struct region_data
//! \brief Expanded read-only \ref spritesheet_region container
//! \details Provides further details about the imported region
struct region_data
{
    std::string name;         //!< Name as specified by import
    std::string type;         //!< Type as specified by import
    spritesheet_region value; //!< Core region data, including size, uv coords, etc.

    //! \brief Collection of tile objects
    //! \details Used with object sheets, tile objects are shapes for creating the
    //!          collision data for the provided region.
    //! \note The position of the object is relative to the region.
    std::vector<tilemap::Object> objects;
};

//! \struct animation_data
//! \brief Expanded read-only \ref Animation container
struct animation_data
{
    std::string name;  //!< Name as specified by import
    Animation   value; //!< Imported \ref Animation
};

//! \struct slice_data
//! \brief Sub-region data defined by the import
struct slice_data
{
    std::string name;   //!< Name as specified by import
    rdge::color color;  //!< Color as specified by import
    screen_rect bounds; //!< Slice boundaries relative to the sheet
    bool is_nine_patch; //!< True iff slice should be broken down into nine regions
    screen_rect center; //!< Center rect of the nine-patch
};

//! \class SpriteSheet
//! \brief Load sprite sheet from a json config
//! \details SpriteSheet (aka TextureAtlas) represents the definition of how
//!          pixel data is broken down to individual sprites.  The definition
//!          is parsed from an external json resource, and includes support
//!          for defining the texture regions.
//!
//!          Optional support for parsing Animation and slice data.
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
//!       "name": "frame_name",
//!       "flip": "horizontal"
//!     } ]
//!   } ],
//!   "meta": {
//!     "slices": [ {
//!       "name": "slice_name",
//!       "color": "#0000ffff",
//!       "data": "",
//!       "keys": [ {
//!         "frame": 0,
//!         "bounds": { "x": 0, "y": 0, "w": 48, "h": 48 },
//!         "center": { "x": 16, "y": 16, "w": 16, "h": 16 }
//!       } ]
//!     } ]
//!   }
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
    std::vector<slice_data> slices;         //!< Slice definition list

    shared_asset<Surface> surface; //!< Pixel data of the sprite sheet
};

//! \brief spritesheet_region stream output operator
std::ostream& operator<< (std::ostream&, const spritesheet_region&);

} // namespace rdge
