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
    screen_rect clip;

    //! \brief Normalized texture coordinates
    tex_coords coords;

    //! \brief Original size of the asset (in pixels)
    math::vec2 size;

    //! \brief Trimmed margin from the original size (in pixels)
    math::vec2 sprite_offset;

    //! \brief Actual size after trimming (in pixels)
    math::vec2 sprite_size;

    //! \brief Pivot origin normalized to the sprite size
    //! \note Defaults to the centroid.
    // TODO Once I do animation, figure out if the origin should be recalculated
    //      for the sprite_size rather than just use the same ratio for the size.
    math::vec2 origin;

    bool is_rotated; //!< Whether the TexturePacker rotated the region 90 clockwise

    //!@{ Basic spritesheet_region transforms
    void flip (TexCoordsFlip) noexcept;
    void rotate (TexCoordsRotation) noexcept;
    void scale (float) noexcept;
    //!@}
};

//! \brief spritesheet_region stream output operator
std::ostream& operator<< (std::ostream&, const spritesheet_region&);

} // namespace rdge
