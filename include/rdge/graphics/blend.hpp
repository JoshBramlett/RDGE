//! \headerfile <rdge/graphics/blend.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/07/2017

#pragma once

#include <rdge/core.hpp>

#include <GL/glew.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum BlendFactor
//! \brief Types of blending functions
//! \details Definitions for how blending is performed.
enum class BlendFactor : uint32
{
    Zero                     = GL_ZERO,                //!< [0, 0, 0, 0]
    One                      = GL_ONE,                 //!< [1, 1, 1, 1]
    SourceColor              = GL_SRC_COLOR,           //!< [s.r, s.g, s.b, s.a]
    OneMinusSourceColor      = GL_ONE_MINUS_SRC_COLOR, //!< [1, 1, 1, 1] - [s.r, s.g, s.b, s.a]
    DestinationColor         = GL_DST_COLOR,           //!< [d.r, d.g, d.b, d.a]
    OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR, //!< [1, 1, 1, 1] - [d.r, d.g, d.b, d.a]
    SourceAlpha              = GL_SRC_ALPHA,           //!< [s.a, s.a, s.a, s.a]
    OneMinusSourceAlpha      = GL_ONE_MINUS_SRC_ALPHA, //!< [1, 1, 1, 1] - [s.a, s.a, s.a, s.a]
    DestinationAlpha         = GL_DST_ALPHA,           //!< [d.a, d.a, d.a, d.a]
    OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA  //!< [1, 1, 1, 1] - [d.a, d.a, d.a, d.a]
};

//! \enum BlendEquation
//! \brief Types of blending equations
//! \details Math operations as to how the source is combined with the destination.
//!          For Add, Subtract, and ReverseSubtract the blending parameters are
//!          multiplied to the source and destination before the operation.
enum class BlendEquation : uint32
{
    Add             = GL_FUNC_ADD,              //!< Source plus destination
    Subtract        = GL_FUNC_SUBTRACT,         //!< Source minus destination
    ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT, //!< Destination minus source
    Min             = GL_MIN,                   //!< Component wise min(src, dest)
    Max             = GL_MAX                    //!< Component wise max(src, dest)
};

//! \class Blend
//! \brief Defines blending behavior
//! \details Blending defines how colors being drawn are mixed with the colors already
//!          in the color buffer.
//! \note Blending is disabled by default.
//! \see http://www.andersriggelsen.dk/glblendfunc.php
class Blend
{
public:
    //! \brief Blend based on the source alpha
    //! \details Linear interpolation between the source and destination colors based
    //!          on the source alpha.
    //! \note Initialized to { SourceAlpha, OneMinusSourceAlpha, One, Zero }
    static const Blend LerpSourceAlpha;

    //! \brief Blend based on both source and destination alphas
    //! \details Linear interpolation between the source and destination colors where
    //!          the source alpha is blended with the destination alpha.
    //! \note Initialized to { SourceAlpha, OneMinusSourceAlpha, One, OneMinusSourceAlpha }
    static const Blend LerpSourceDestAlpha;

    //! \brief Blend default ctor
    Blend (void) = default;

    //! \brief Blend ctor
    //! \details Apply source and destination parameters to both color and alpha.
    //! \param [in] source Source parameter
    //! \param [in] destination Destination parameter
    explicit Blend (BlendFactor source, BlendFactor destination);

    //! \brief Blend ctor
    //! \details Source and destination parameters, separating color from the alpha.
    //! \param [in] source_rgb Source RGB parameter
    //! \param [in] destination_rgb Destination RGB parameter
    //! \param [in] source_alpha Source alpha parameter
    //! \param [in] destination_alpha Destination alpha parameter
    explicit Blend (BlendFactor source_rgb,
                    BlendFactor destination_rgb,
                    BlendFactor source_alpha,
                    BlendFactor destination_alpha);

    //! \brief Apply cached blend settings
    //! \details Calls OpenGL to enable/disable blending, and if enabled will provide
    //!          OpenGL with the blennding function and equation.
    //! \note The current blending state is globally cached so applying will only
    //!       send the changes to OpenGL if there is a state change or if the values
    //!       differ from the cached version.
    void Apply (void) const;

public:
    bool enabled = false; //!< Blending is enabled

    BlendFactor src_rgb   = BlendFactor::One;  //!< Source RGB factor
    BlendFactor dst_rgb   = BlendFactor::Zero; //!< Destination RGB factor
    BlendFactor src_alpha = BlendFactor::One;  //!< Source alpha factor
    BlendFactor dst_alpha = BlendFactor::Zero; //!< Destination alpha factor

    BlendEquation mode_rgb   = BlendEquation::Add; //!< RGB blend equation
    BlendEquation mode_alpha = BlendEquation::Add; //!< Alpha blend equation
};

//! \brief Blend equality operator
//! \param [in] lhs Left side Blend to compare
//! \param [in] rhs Right side Blend to compare
//! \returns True iff blend objects are identical
inline bool operator== (const Blend& lhs, const Blend& rhs) noexcept
{
    return (lhs.enabled == rhs.enabled) &&
           (lhs.src_rgb == rhs.src_rgb) &&
           (lhs.dst_rgb == rhs.dst_rgb) &&
           (lhs.src_alpha == rhs.src_alpha) &&
           (lhs.dst_alpha == rhs.dst_alpha) &&
           (lhs.mode_rgb == rhs.mode_rgb) &&
           (lhs.mode_alpha == rhs.mode_alpha);
}

//! \brief Blend inequality operator
//! \param [in] lhs Left side vec3 to compare
//! \param [in] rhs Right side vec3 to compare
//! \returns True iff blend objects are not identical
inline bool operator!= (const Blend& lhs, const Blend& rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace rdge
