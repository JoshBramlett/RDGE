//! \headerfile <rdge/assets/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/graphics/isprite.hpp>

#include <memory>
#include <string>
#include <unordered_map>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace assets {

//! \class SpriteSheet
//! \brief Load sprite sheet from config file
//! \details Config file contains the make up for the sprite sheet, which
//!          includes the image, and the coordinates for parsing the image
//!          into texture coordinates.  The "pos" and "size" values must be
//!          in pixel perfect coordinates, and the uv data returned will be
//!          in decimal format utilized by OpenGL.
//! \note OpenGL highly recommends asset dimensions be in powers of two,
//!       therefore the parse will fail for an NPOT image.
//! \code{.json}
//! {
//!     "file": "textures/image.png",
//!     "uv": [ {
//!         "name": "MY_UV_1",
//!         "pos": [0, 0],
//!         "size": [16, 16]
//!     } ]
//! }
//! \endcode
class SpriteSheet
{
public:
    //! \brief SpriteSheet ctor
    //! \details Loads and parses the config file.
    //! \param [in] path Path to the config file
    //! \throws rdge::Exception Unable to parse config
    explicit SpriteSheet (const std::string& path);

    //! \brief SpriteSheet dtor
    ~SpriteSheet (void) noexcept = default;

    //! \brief SpriteSheet Copy ctor
    //! \details Non-copyable
    SpriteSheet (const SpriteSheet&) = delete;

    //! \brief SpriteSheet Move ctor
    //! \details Default-movable
    SpriteSheet (SpriteSheet&&) noexcept = default;

    //! \brief SpriteSheet Copy Assignment Operator
    //! \details Non-copyable
    SpriteSheet& operator= (const SpriteSheet&) = delete;

    //! \brief SpriteSheet Move Assignment Operator
    //! \details Default-movable
    SpriteSheet& operator= (SpriteSheet&&) noexcept = default;

    //! \brief SpriteSheet Subscript Operator
    //! \details Retrieves the sprite uv data by name
    //! \param [in] name Name of the element
    //! \returns Associated tex_coords
    //! \throws rdge::Exception Element is not located
    rdge::tex_coords operator[] (const std::string& name);

    //! \brief Get the surface specified in the config
    //! \returns Shared pointer to the loaded surface
    std::shared_ptr<rdge::Surface> GetSurface (void) const;

private:
    std::shared_ptr<rdge::Surface> m_surface;
    std::unordered_map<std::string, rdge::tex_coords> m_elements;
};

} // namespace assets
} // namespace rdge
