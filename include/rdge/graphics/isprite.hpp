//! \headerfile <rdge/graphics/isprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <array>
#include <cmath>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

class SpriteBatch; //!< Forward declaration


//! \struct sprite_vertex
//! \brief The organization of sprite vertex attributes
struct sprite_vertex
{
    math::vec3 pos;                              //!< Vertex coordinates
    math::vec2 uv;                               //!< Texture (UV) coordinates
    uint32     tid   = Texture::INVALID_UNIT_ID; //!< Texture Unit ID
    uint32     color = 0xFFFFFFFF;               //!< Vertex color
};

//! \typedef Array of sprite vertex attributes
using SpriteVertices = std::array<sprite_vertex, 4>;

//! \struct tex_coords
//! \brief Texture coordinate (UV) data for a sprite
//! \details Default values are set to fill the entire texture.
struct tex_coords
{
    math::vec2 top_left     = { 0.f, 1.f };
    math::vec2 bottom_left  = { 0.f, 0.f };
    math::vec2 bottom_right = { 1.f, 0.f };
    math::vec2 top_right    = { 1.f, 1.f };

    //!@{
    //! \brief Subscript operators to access an element by index
    math::vec2& operator[] (uint32 index) noexcept;
    const math::vec2& operator[] (uint32 index) const noexcept;
    //!@}

    //!@{
    //! \brief Basic tex_coords transforms
    //! \returns Reference to self
    tex_coords& flip_horizontal (void) noexcept;
    tex_coords& flip_vertical (void) noexcept;
    tex_coords& rotate_left (void) noexcept;
    tex_coords& rotate_right (void) noexcept;
    //!@}
};

using SpriteDepthOffset = std::integral_constant<uint32, 10'000>;
using LayerDepthOffset  = std::integral_constant<uint32, 100>;

//! \struct smart_zindex
//! \brief Utility for converting an integer z-index value to a floating point mask
//! \details Should be used in a parent/child relationship (e.g. Layer and Sprites).
//!          The parent could consume the first two digits of the mantissa, and the
//!          child could consume digits 3-4, which when added together  allows the
//!          child to inherit the relative depth from their parent.
template <uint32 Offset>
struct smart_zindex
{
    // TODO This is somewhat limiting, as it's tied to an orthographic projection without
    //      knowing what the near/far values that are set.  Also it only has support for
    //      positive values

    static constexpr uint32 LSD     = Offset; //!< Least significant floating point digit
    static constexpr uint32 MAX_VAL = 99;     //!< Maximum z-index integer value

    //! \brief Convert a integer z-index to a floating point mask
    //! \param [in] value z-index value
    //! \returns Floating point mask
    static constexpr float32 convert (uint32 value)
    {
        return static_cast<float>((value < MAX_VAL) ? value : MAX_VAL) /
               static_cast<float>(LSD);
    }
};

//! \class ISprite
//! \brief Interface for 2D sprites
class ISprite
{
public:
    using DepthMask = smart_zindex<SpriteDepthOffset::value>; //!< Depth mask conversion

    //! \brief ISprite dtor
    virtual ~ISprite (void) noexcept = default;

    //! \brief Draw sprite with the provided render target
    //! \param [in] renderer Render target
    virtual void Draw (SpriteBatch& renderer) = 0;

    //! \brief Register sprite to the provided render target
    //! \param [in] renderer Render target
    virtual void SetRenderTarget (SpriteBatch& renderer) = 0;

    //! \brief Requests the sprite to add the provided mask to it's own
    //! \details The mask represents two digits of a floating point mantissa.
    //! \param [in] mask Depth mask
    //! \see smart_zindex
    virtual void AmendDepthMask (float32 mask) = 0;
};

} // namespace rdge
