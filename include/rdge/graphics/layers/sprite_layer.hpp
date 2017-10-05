//! \headerfile <rdge/graphics/layers/sprite_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/26/2016

#pragma once

#include <rdge/core.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class ISprite;
class Shader;
class SpriteBatch;
//!@}

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
    //! \brief SpriteLayer default ctor
    //! \details Creates a new instance of the render target.
    //! \param [in] num_sprites Max number of sprites that can be submitted
    //! \param [in] shader Shader to pass to the render target
    explicit SpriteLayer (uint16 num_sprites = 1000, std::shared_ptr<Shader> shader = nullptr);

    //! \brief SpriteLayer ctor
    //! \details Uses an existing render target.
    //! \param [in] render_target Shared render target
    // TODO If multiple layers share a renderer each still does it own prep and flush.
    //      I could easily avoid this by setting a flag on the draw call.  Make sure
    //      to profile the impact.
    explicit SpriteLayer (std::shared_ptr<SpriteBatch> render_target);

    //! \brief SpriteLayer dtor
    ~SpriteLayer (void) noexcept = default;

    //!@{ Non-copyable, move enabled
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
    std::shared_ptr<SpriteBatch>          renderer; //!< Render target
    std::vector<std::shared_ptr<ISprite>> sprites;  //!< Collection of sprites
};

} // namespace rdge
