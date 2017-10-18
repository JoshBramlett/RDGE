//! \headerfile <rdge/graphics/isprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <array>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SpriteBatch;
//!@}

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

//! \class ISprite
//! \brief Interface for 2D sprites
class ISprite
{
public:
    //! \brief ISprite dtor
    virtual ~ISprite (void) noexcept = default;

    //! \brief Register sprite to the provided render target
    //! \param [in] renderer Render target
    virtual void SetRenderTarget (SpriteBatch& renderer) = 0;

    //! \brief Draw sprite with the provided render target
    //! \param [in] renderer Render target
    virtual void Draw (SpriteBatch& renderer) = 0;

    //! \brief Set the sprites depth value
    //! \param [in] depth Depth (z-index) value
    virtual void SetDepth (float depth) = 0;
};

//! \brief SpriteVertices stream output operator
std::ostream& operator<< (std::ostream& os, const SpriteVertices& value);

} // namespace rdge
