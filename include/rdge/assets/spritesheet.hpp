//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/sprite.hpp>

#include <memory>
#include <string>
#include <unordered_map>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class texture_part
//! \brief Represents an individual section of the \ref SpriteSheet
struct texture_part
{
    std::string  name;   //!< Unique name
    math::uivec2 size;   //!< Original size in pixels
    tex_coords   coords; //!< Normalized texture coordinates
};

//! \class SpriteSheet
//! \brief Load sprite sheet from a json formatted config file
//! \details Config file contains the make up for the sprite sheet, which
//!          includes the image path to load and the coordinates that will
//!          make up the parts.  Coordinates are required to be zero index
//!          pixel perfect unsigned integers with the origin being set to
//!          the top left corner and will be normalized to inverted floating
//!          point texture coordinates.
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
//!         "height": 32
//!     } ]
//! }
//! \endcode
class SpriteSheet
{
public:
    //! \brief SpriteSheet ctor
    SpriteSheet (void) = default;

    //! \brief SpriteSheet ctor
    //! \details Loads and parses the config file.
    //! \param [in] path Path to the config file
    //! \throws rdge::Exception Unable to parse config
    explicit SpriteSheet (const std::string& path);

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

    //! \brief Create a sprite from the sub-texture element
    //! \details Uses the provided position and size along with the associated
    //!          uv data from the name lookup to construct a sprite.
    //! \param [in] name Name of the element
    //! \param [in] pos Sprite position
    //! \param [in] size Sprite size
    //! \returns Sprite object
    //! \throws rdge::Exception Lookup failed
    std::unique_ptr<Sprite> CreateSprite (const std::string& name, const math::vec3& pos) const;

public:
    std::shared_ptr<Surface> surface; //!< Surface created from image
    std::shared_ptr<Texture> texture; //!< Texture generated from the surface

private:
    std::unordered_map<std::string, texture_part> m_parts; //!< Collection of texture parts
};

} // namespace rdge
