//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/animation.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/rect.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/sprite_group.hpp>
#include <rdge/graphics/sprite_group.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{
//! \brief Forward declarations
class Animation;
//!@}

//! \class texture_part
//! \brief Represents an individual section of the \ref SpriteSheet
//! \details Container includes the data the client can use for rendering and
//!          commonly represents a sprite texture or alternatively a single
//!          animation frame.  The hotspot is an optional field in the config
//!          intended to be used to align animation frames which may have a
//!          different size.
//! \note The size and hotspot values may be modified from the config to
//!       accommodate the scale multiplication.
struct texture_part
{
    std::string  name;    //!< Unique name
    screen_rect  clip;    //!< Unmodified clipping rectangle
    math::uivec2 size;    //!< Size in pixels (scaled)
    math::uivec2 hotspot; //!< Origin used for drawing offsets (scaled)
    tex_coords   coords;  //!< Normalized texture coordinates

    //!@{
    //! \brief Basic tex_coords transforms
    //! \returns Reference to self
    texture_part& flip_horizontal (void) noexcept;
    texture_part& flip_vertical (void) noexcept;
    //!@}

    //!@{
    //! \brief Basic tex_coords transforms
    //! \returns Value after transform
    texture_part flip_horizontal (void) const noexcept;
    texture_part flip_vertical (void) const noexcept;
    //!@}
};

//! \class SpriteSheet
//! \brief Load sprite sheet from a json formatted config file
//! \details Config file contains the make up for the sprite sheet, which
//!          includes the image path to load and the coordinates that will make
//!          up the parts.  Coordinates are required to be zero index pixel
//!          perfect unsigned integers with the origin being set to the top
//!          left corner and will be normalized to inverted floating point
//!          texture coordinates.  The image_scale is an optional parameter
//!          which is a scale multiplier applied to all texture parts.  This
//!          is useful for small pixel art.  Specifying a hotspot is optional
//!          whose coordinates should be set relative to the texture_part,
//!          not the spritesheet.
//!          Animations are optional and will create an \ref Animation object
//!          with the frames made up of the texture_part definitions in the
//!          same file.
//! \note No checking is done to ensure parts are unique or do not overlap.
//! \warning No type conversions are performed, so numeric values wrapped in
//!          quotes are treated as strings and the parsing will fail.
//! \code{.json}
//! {
//!     "image_path": "textures/image.png",
//!     "image_scale": 4,
//!     "texture_parts": [ {
//!         "name": "part_name",
//!         "x": 0,
//!         "y": 0,
//!         "width": 32,
//!         "height": 32,
//!         "hotspot_x": 0,
//!         "hotspot_y": 0
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
    //! \details Loads and parses the config file.  If the sprite sheet was designed
    //!          for use with a standard display and ran using (and configured for)
    //!          a hi-res display, setting the scale_for_hires flag to true will scale
    //!          the texture_part.size value by a multiple of 2.
    //! \param [in] path Path to the config file
    //! \param [in] scale_for_hidpi Scale original image size for hi-resolution display
    //! \throws rdge::Exception Unable to parse config
    explicit SpriteSheet (const std::string& path, bool scale_for_hidpi = false);

    //! \brief SpriteSheet dtor
    ~SpriteSheet (void) noexcept = default;

    //!@{
    //! \brief Non-copyable and move enabled
    SpriteSheet (const SpriteSheet&) = delete;
    SpriteSheet& operator= (const SpriteSheet&) = delete;
    SpriteSheet (SpriteSheet&&) noexcept = default;
    SpriteSheet& operator= (SpriteSheet&&) noexcept = default;
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

    //! \brief Create a sprite from the sub-texture element
    //! \details Uses the provided position and size along with the associated
    //!          uv data from the name lookup to construct a sprite.
    //! \param [in] name Name of the element
    //! \param [in] pos Sprite position
    //! \returns Sprite unique pointer
    //! \throws rdge::Exception Lookup failed
    std::unique_ptr<Sprite> CreateSprite (const std::string& name, const math::vec3& pos) const;

    //! \brief Create a chain of sprites from the sub-texture element
    //! \details Similar to \ref CreateSprite, but will create a \ref SpriteGroup
    //!          by chaining together the same sprite.  The intended purpose is to
    //!          overcome the limitation imposed by OpenGL where texture wrapping
    //!          cannot be done when specifying sub-texture uv coordinates.
    //! \note If only a single dimension of the intended final size is known a
    //!       value of zero can be passed for the other dimension.
    //! \param [in] name Name of the element
    //! \param [in] pos Sprite position
    //! \param [in] to_fill The intended final size
    //! \returns SpriteGroup unique pointer
    //! \throws rdge::Exception Lookup failed
    std::unique_ptr<SpriteGroup> CreateSpriteChain (const std::string& name,
                                                    const math::vec3&  pos,
                                                    const math::vec2&  to_fill) const;

public:
    std::string image_path;      //!< Image path specified in the config
    uint32      image_scale = 1; //!< Image scale specified in the config

    std::shared_ptr<Surface> surface; //!< Surface created from image
    std::shared_ptr<Texture> texture; //!< Texture generated from the surface

private:
    std::unordered_map<std::string, texture_part> m_parts;   //!< Collection of texture parts
    std::unordered_map<std::string, Animation> m_animations; //!< Collection of animations
};

} // namespace rdge
