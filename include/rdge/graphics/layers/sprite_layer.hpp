//! \headerfile <rdge/graphics/layers/sprite_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/26/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/math/mat4.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class SpriteLayer
//! \brief Layer of ISprite objects
//! \details A layer represents a logical group of sprites that will be drawn
//!          with the same render target.  Facilitates the interface between the
//!          sprite and the render target including providing batch drawing.
//!          Setting a z-index for the layer will assign the z position component
//!          to all maintained renderables that occupies the two most significant
//!          digits of the mantissa.
class SpriteLayer
{
public:
    using DepthMask = smart_zindex<LayerDepthOffset::value>; //!< Depth mask conversion

    //! \brief SpriteLayer ctor
    //! \details Provided values will be forwarded to the render target
    //! \param [in] num_sprites Max number of sprites that can be submitted
    //! \param [in] shader Shader to pass to the render target
    explicit SpriteLayer (uint16 num_sprites = 1000, std::shared_ptr<Shader> shader = nullptr);

    //! \brief SpriteLayer dtor
    ~SpriteLayer (void) noexcept = default;

    //!@{
    //! \brief Non-copyable, move enabled
    SpriteLayer (const SpriteLayer&) = delete;
    SpriteLayer& operator= (const SpriteLayer&) = delete;
    SpriteLayer (SpriteLayer&&) noexcept;
    SpriteLayer& operator= (SpriteLayer&&) noexcept;
    //!@}

    //! \brief Add sprite to the cache
    //! \param [in] sprite Shared ISprite object
    //! \param [in] generate_sprite_depth Assign sprite a z-index
    void AddSprite (std::shared_ptr<ISprite> sprite);

    //! \brief Draw all cached sprites
    void Draw (void);

public:
    SpriteBatch renderer;                          //!< Render target
    uint32      z_index = 0;                       //!< Orthographic projection depth
    std::vector<std::shared_ptr<ISprite>> sprites; //!< Collection of sprites
};

} // namespace rdge
