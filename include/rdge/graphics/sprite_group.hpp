//! \headerfile <rdge/graphics/sprite_group.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/19/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/math/mat4.hpp>

#include <memory>
#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class SpriteGroup
//! \brief Represents a group of \ref ISprite objects
//! \details Used for logical grouping and performing a similar transformation
//!          on all sprites within the collection.
class SpriteGroup : public ISprite
{
public:
    //! \brief SpriteGroup ctor
    //! \details The transformation is defaulted to an indentity matrix to
    //!          allow for logical ordering without processing a transformation.
    //! \param [in] transform Transformation to apply
    explicit SpriteGroup (const math::mat4& transform = math::mat4::identity());

    //! \brief SpriteGroup dtor
    ~SpriteGroup (void) noexcept = default;

    //!@{
    //! \brief Copy and move enabled
    SpriteGroup (const SpriteGroup&) = default;
    SpriteGroup& operator= (const SpriteGroup&) = default;
    SpriteGroup (SpriteGroup&&) noexcept = default;
    SpriteGroup& operator= (SpriteGroup&&) noexcept = default;
    //!@}

    //! \brief Register the texture with the render target
    //! \details This step must be performed prior to making a draw call.
    //! \param [in] renderer Render target
    void SetRenderTarget (SpriteBatch& renderer) override;

    //! \brief Draw the sprite using the provided renderer
    //! \param [in] renderer Render target
    void Draw (SpriteBatch& renderer) override;

    //! \brief Set the sprites depth value
    //! \param [in] depth Depth (z-index) value
    void SetDepth (float depth) override;

    //! \brief Add sprite to the cache
    //! \param [in] sprite Shared ISprite object
    void AddSprite (std::shared_ptr<ISprite> sprite);

    std::vector<std::shared_ptr<ISprite>> sprites; //!< Collection of sprites
    math::mat4 transformation; //!< Affine transform applied to all sprites before drawing
};

} // namespace rdge
