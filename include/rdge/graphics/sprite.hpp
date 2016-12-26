//! \headerfile <rdge/graphics/sprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/23/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Sprite
//! \brief Represents a 2D renderable sprite
class Sprite : public ISprite
{
public:
    SpriteVertices vertices; //!< Array of vertex attribute data

    //! \brief Sprite ctor
    //! \details Creates a sprite rendered with the provided color.
    //! \param [in] pos Sprite position
    //! \param [in] size Sprite size
    //! \param [in] color Color of all vertices
    explicit Sprite (const math::vec3& pos, const math::vec2& size, const color& color);

    //! \brief Sprite ctor
    //! \details Creates a sprite rendered using the provided texture.  If no texture
    //!          coordinates are provided it will be set to span the entire texture.
    //! \param [in] pos Sprite position
    //! \param [in] size Sprite size
    //! \param [in] texture Texture the sprite will use
    //! \param [in] uv Texture (uv) coordinates
    explicit Sprite (const math::vec3&        pos,
                     const math::vec2&        size,
                     std::shared_ptr<Texture> texture,
                     const tex_coords&        coords = tex_coords());

    //! \brief Sprite dtor
    ~Sprite (void) noexcept = default;

    //!@{
    //! \brief Copy and move enabled
    Sprite (const Sprite&) noexcept = default;
    Sprite& operator= (const Sprite&) noexcept = default;
    Sprite (Sprite&&) noexcept;
    Sprite& operator= (Sprite&&) noexcept;
    //!@}

    //! \brief Draw the sprite using the provided renderer
    //! \param [in] renderer Render target
    void Draw (SpriteBatch& renderer) override;

    //! \brief Register the texture with the render target
    //! \details This step must be performed prior to making a draw call.
    //! \param [in] renderer Render target
    void SetRenderTarget (SpriteBatch& renderer) override;

private:
    std::shared_ptr<Texture> m_texture; //!< Texture associated with the sprite
};

} // namespace rdge
