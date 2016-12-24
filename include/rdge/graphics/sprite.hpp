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

/* TODO From Renderer2D - will implement on an as-needed basis
 *
 * After some thought (and going through text.cpp), instead of the OO model
 * so I wouldn't have to reinvent the wheel for common functions, really the
 * only thing in common is the SpriteVertices.  So why not have helpers
 * that work only on that object?  So, I don't need to implement as needed,
 * but instead create a function that will work on these.  This adds the benefit
 * that if I need batching/simd, it'll be significantly easier.

    //! \brief Set the position of the renderable
    //! \param [in] position vec3 representing the position
    void SetPosition (const rdge::math::vec3& position);

    //! \brief Set the size of the renderable
    //! \param [in] size vec2 representing the size
    void SetSize (const rdge::math::vec2& size);

    //! \brief Set the color of the renderable
    //! \param [in] color Color of the renderable
    //! \param [in] ignore_alpha True will only set the RGB values
    void SetColor (const rdge::color& color, bool ignore_alpha = true);

    //! \brief Set the texture UV coordinates of the renderable
    //! \param [in] uv UVCoordinates object
    void SetUV (const UVCoordinates& uv);

    //! \brief Set the opacity of the renderable
    //! \details Maps to the alpha value of the color
    //! \param [in] opacity Alpha channel value [0-255]
    void SetOpacity (rdge::uint8 opacity);

    //! \brief Set the opacity of the renderable
    //! \details Maps to the alpha value of the color
    //! \param [in] opacity Alpha channel value [0.0f-1.0f]
    void SetOpacity (float opacity);

    //! \brief Set the Z-Index position value
    //! \param [in] zindex Z-Index value
    void SetZIndex (float zindex);

    //! \brief Scale the size by the provided multiplier
    //! \details Current size is a value of 1.0.
    //! \param [in] scaler Scale value
    //! \throws RDGE::Exception Scaler is not a positive value
    void Scale (float scaler);
*/

private:
    std::shared_ptr<Texture> m_texture; //!< Texture associated with the sprite
};

} // namespace rdge
