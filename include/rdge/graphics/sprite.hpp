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
    //! \brief Sprite ctor
    //! \details Creates a sprite rendered with the provided color.
    //! \param [in] pos Sprite position
    //! \param [in] size Sprite size
    //! \param [in] color Color of all vertices
    explicit Sprite (const math::vec3& pos, const math::vec2& size, const color& color);

    //! \brief Sprite ctor
    //! \details Creates a sprite rendered using the provided texture.  The size is
    //!          derived from the texture so the sprite must consume the entire texture.
    //! \param [in] pos Sprite position
    //! \param [in] texture Texture the sprite will use
    explicit Sprite (const math::vec3& pos, std::shared_ptr<Texture> texture);

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
    Sprite (Sprite&&) noexcept = default;
    Sprite& operator= (Sprite&&) noexcept = default;
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

public:
    SpriteVertices vertices; //!< Array of vertex attribute data

#if RDGE_DEBUG
    //! \struct sprite_debug_bounds
    //! \brief Draw a solid colored box underneath the sprite
    //! \details Provides a visual for the sprite drawing bounds, intended for
    //!          use with sprites rendered using a partially transparent texture.
    //!          The bounding box is constructed every frame using the coordinates
    //!          of the sprite, including the depth value.
    //! \note Only available in debug builds.
    //! \warning Enabling increases the submission count, so before use ensure
    //!          the renderer's capacity can accomodate the extra submission.
    struct sprite_debug_bounds
    {
        bool show = false;                //!< Show the debug bounds
        color draw_color = color::YELLOW; //!< Color of the debug bounds

    } debug_bounds;
#endif

private:
    std::shared_ptr<Texture> m_texture; //!< Texture associated with the sprite
};

} // namespace rdge
