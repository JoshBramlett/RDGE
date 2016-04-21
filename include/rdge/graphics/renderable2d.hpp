//! \headerfile <rdge/graphics/renderable2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/02/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/color.hpp>

#include <memory>
#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Renderable2D
//! \brief Base class for a 2D object to be rendered to the screen
class Renderable2D
{
public:
    //! \typedef UVCoordinates Collection of UV coordinates
    using UVCoordinates = std::vector<RDGE::Math::vec2>;

    //! \brief Renderable2D default empty ctor
    Renderable2D (void)
        : m_texture(nullptr)
        , m_uv(DefaultUVCoordinates())
    { }

    //! \brief Renderable2D ctor
    //! \param [in] position Location coordinates.  Z-axis is for layering
    //! \param [in] size Width and height of the renderable
    explicit Renderable2D (const RDGE::Math::vec3& position, const RDGE::Math::vec2& size)
        : m_texture(nullptr)
        , m_uv(DefaultUVCoordinates())
        , m_position(position)
        , m_size(size)
    { }

    //! \brief Renderable2D dtor
    virtual ~Renderable2D (void) { }

    //! \brief Submit renderable to the renderer
    //! \details Base class implementation that simply performs the submission.
    //! \param [in] renderer Renderer2D to submit to
    virtual void Submit (Renderer2D* renderer) const
    {
        renderer->Submit(this);
    }

    //! \brief Get the position of the renderable object.
    //! \returns vec3 of coordinates
    const RDGE::Math::vec3& Position (void) const noexcept
    {
        return m_position;
    }

    //! \brief Get the size of the renderable object.
    //! \returns vec2 of the width and height
    const RDGE::Math::vec2& Size (void) const noexcept
    {
        return m_size;
    }

    //! \brief Get the color of the renderable object.
    //! \returns Fill color
    const RDGE::Color& Color (void) const noexcept
    {
        return m_color;
    }

    //! \brief Get the shader unit id of the renderable object
    //! \returns Unit id, or -1 if not available
    RDGE::Int32 TextureUnitID (void) const noexcept
    {
        return m_texture ? static_cast<RDGE::Int32>(m_texture->UnitID()) : -1;
    }

    std::shared_ptr<GLTexture> Texture (void) const noexcept
    {
        return m_texture;
    }

    //! \brief Get the texture coordinates of the renderable object.
    //! \returns Vector of coordinates
    const std::vector<RDGE::Math::vec2>& UV (void) const noexcept
    {
        return m_uv;
    }

    void SetSize (const RDGE::Math::vec2& size)
    {
        m_size = size;
    }

    //! \brief Set the Z-Index position value
    //! \param [in] zindex Z-Index value
    void SetZIndex (float zindex)
    {
        m_position.z = RDGE::Math::clamp(zindex, 0.0f, 1.0f);
    }

    //! \brief Get the default UV coordinates
    //! \details The default UV coordinates makeup the entire texture.  The
    //!          ordering of the coordinates is important and must follow
    //!          in counter-clockwise starting with the top left coordinate.
    //! \returns UV coordinates container with default values
    static UVCoordinates DefaultUVCoordinates (void)
    {
        return UVCoordinates {
            RDGE::Math::vec2(0, 1),
            RDGE::Math::vec2(0, 0),
            RDGE::Math::vec2(1, 0),
            RDGE::Math::vec2(1, 1)
        };
    }

protected:
    RDGE::Color m_color;

    std::shared_ptr<GLTexture>    m_texture;
    std::vector<RDGE::Math::vec2> m_uv;

private:
    RDGE::Math::vec3 m_position;
    RDGE::Math::vec2 m_size;
};

} // namespace Graphics
} // namespace RDGE
