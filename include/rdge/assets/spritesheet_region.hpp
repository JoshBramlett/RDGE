//! \headerfile <rdge/assets/spritesheet_region.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct spritesheet_region
//! \brief Represents an individual section of the \ref SpriteSheet
//! \details Container includes the data the client can use for rendering and
//!          commonly represents a sprite texture or animation frame.
//! \note The size and origin values may be modified from the config to
//!       accommodate the scale multiplication.
struct spritesheet_region
{
    //! \brief Surface clipping rectangle
    //! \details Should remain unmodified from the import.  Used for blitting
    //!          from the associated surface.
    screen_rect clip;

    //! \brief Normalized texture coordinates
    //! \details UV coordinates normalized to the associated surface.
    tex_coords coords;

    //! \brief Size (possibly scaled) of the texture region
    //! \details Used as a cache for rendering operations to prevent having to
    //!          compute a scaled size every frame.
    math::vec2 size;

    //! \brief Origin (possibly scaled) used for drawing offsets
    //! \details Optional field for aligning similar frames.  The origin
    //!          represents the center point when generating sprite vertices.
    //! \note Defaults to the centroid.
    math::vec2 origin;

    //!@{ Basic spritesheet_region transforms
    void flip (TexCoordsFlip) noexcept;
    void rotate (TexCoordsRotation) noexcept;
    void scale (float) noexcept;
    //!@}
};

//! \brief spritesheet_region stream output operator
std::ostream& operator<< (std::ostream&, const spritesheet_region&);

} // namespace rdge
