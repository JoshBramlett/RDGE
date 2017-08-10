//! \headerfile <rdge/assets/texture_part.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/09/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/types.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class texture_part
//! \brief Represents an individual section of the \ref SpriteSheet
//! \details Container includes the data the client can use for rendering and
//!          commonly represents a sprite texture or alternatively a single
//!          animation frame.
//! \note The size and origin values may be modified from the config to
//!       accommodate the scale multiplication.
struct texture_part
{
    std::string  name;   //!< Unique name
    screen_rect  clip;   //!< Unmodified clipping rectangle
    tex_coords   coords; //!< Normalized texture coordinates
    math::uivec2 size;   //!< Size in pixels (scaled)

    //! \brief Origin used for drawing offsets (scaled)
    //! \details Optional field used to align frames, and to generate sprite
    //!          vertices from a world center point.  Value is defaulted to
    //!          the centroid if unset.
    math::vec2 origin;

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

//! \brief texture_part stream output operator
std::ostream& operator<< (std::ostream&, const texture_part&);

} // namespace rdge
