//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/texture_part.hpp>
#include <rdge/graphics/animation.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Sprite;
class SpriteGroup;
class Surface;
class Texture;
//!@}

//! \class SpriteSheet
//! \brief Load sprite sheet from a json formatted config file
//! \details Config file contains the make up for the sprite sheet, which
//!          includes the image path to load and the coordinates that will make
//!          up the parts.  Coordinates are required to be zero index pixel
//!          perfect unsigned integers with the origin being set to the top
//!          left corner and will be normalized to inverted floating point
//!          texture coordinates.  Origin is an optional field that if set can
//!          be used during rendering to override the logical center of the
//!          texture region.
//!          Animations are optional and will create an \ref Animation object
//!          with the frames made up of the texture_part definitions in the
//!          same file.
//! \note No checking is done to ensure parts are unique or do not overlap.
//! \warning No type conversions are performed, so numeric values wrapped in
//!          quotes are treated as strings and the parsing will fail.
//! \code{.json}
//! {
//!     "image_path": "textures/image.png",
//!     "texture_parts": [ {
//!         "name": "part_name",
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
class SpriteSheet
{
public:
    //! \brief SpriteSheet ctor
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
    const texture_part& operator[] (const std::string& name) const;

    //! \brief Retrive an animation by name
    //! \param [in] name Name of the animation
    //! \returns Associated \ref Animation
    //! \throws rdge::Exception Lookup failed
    const Animation& GetAnimation (const std::string& name) const;

    // TODO
    // - Add getter for the texture_part
    // - Rename texture_part to texture_region
    // - Support scaling?
    // - Check for duplicate keys during import?
    const Animation& GetAnimation (int32 animation_id) const
    {
        return animations[animation_id].value;
    }

    // TODO Remove
    std::unique_ptr<Sprite> CreateSprite (const std::string& name,
                                          const math::vec3& pos) const;

    // TODO Remove
    std::unique_ptr<SpriteGroup> CreateSpriteChain (const std::string& name,
                                                    const math::vec3&  pos,
                                                    const math::vec2&  to_fill) const;

public:
    Surface                  surface; //!< Surface specified in the file input
    std::shared_ptr<Texture> texture; //!< Texture generated from the surface

    //! \struct region_data
    //! \brief Internally used for keying off the name string
    struct region_data
    {
        std::string  name;
        texture_part value;
    };

    //! \struct animation_data
    //! \brief Internally used for keying off the name string
    struct animation_data
    {
        std::string name;
        Animation   value;
    };

    //!@{ Region container
    region_data* regions = nullptr;
    size_t       region_count = 0;
    //!@}

    //!@{ Animation container
    animation_data* animations = nullptr;
    size_t          animation_count = 0;
    //!@}
};

} // namespace rdge
