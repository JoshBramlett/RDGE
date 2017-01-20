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
//!          together using the same render target.  A uniform depth value can be set
//!          for all cached sprites in order to mimic the behavior of a photoshop
//!          layer.  Facilitates the interface between the sprite and the render
//!          target for batch drawing.
class SpriteLayer
{
public:
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
    SpriteLayer (SpriteLayer&&) noexcept = default;
    SpriteLayer& operator= (SpriteLayer&&) noexcept = default;
    //!@}

    //! \brief Add sprite to the cache
    //! \param [in] sprite Shared ISprite object
    void AddSprite (std::shared_ptr<ISprite> sprite);

    //! \brief Draw all cached sprites
    void Draw (void);

    //! \brief Give all cached sprites a uniform depth
    //! \param [in] depth Depth (z-index) value
    void OverrideSpriteDepth (float depth);

public:
    //TODO Consider making renderer a shared_ptr so layers can share a common renderer
    SpriteBatch renderer;                          //!< Render target
    std::vector<std::shared_ptr<ISprite>> sprites; //!< Collection of sprites
};

} // namespace rdge
