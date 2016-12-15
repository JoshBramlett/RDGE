//! \headerfile <rdge/gfx/renderable2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.8
//! \date 06/07/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/gfx/color.hpp>
#include <rdge/gfx/renderer2d.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <SDL.h>

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

//! \typedef UVCoordinates Collection of UV coordinates
using UVCoordinates = std::vector<rdge::math::vec2>;

//! \class Renderable2D
//! \brief Base class for a 2D object to be rendered to the screen
class Renderable2D
{
public:
    //! \brief Renderable2D default empty ctor
    Renderable2D (void)
        : m_texture(nullptr)
        , m_uv(DefaultUVCoordinates())
    { }

    //! \brief Renderable2D ctor
    //! \param [in] position Location coordinates.  Z-axis is for layering
    //! \param [in] size Width and height of the renderable
    //! \note Color is defaulted to white simply so the alpha is set to 255
    explicit Renderable2D (const rdge::math::vec3& position, const rdge::math::vec2& size)
        : m_position(position)
        , m_size(size)
        , m_color(color::WHITE())
        , m_texture(nullptr)
        , m_uv(DefaultUVCoordinates())
    { }

    //! \brief Renderable2D dtor
    virtual ~Renderable2D (void) { }

    //! \brief Renderable2D Copy ctor
    //! \details Non-copyable
    Renderable2D (const Renderable2D&) = delete;

    //! \brief Renderable2D Move ctor
    //! \details Transfers ownership
    Renderable2D (Renderable2D&&) noexcept;

    //! \brief Renderable2D Copy Assignment Operator
    //! \details Non-copyable
    Renderable2D& operator= (const Renderable2D&) = delete;

    //! \brief Renderable2D Move Assignment Operator
    //! \details Transfers ownership
    Renderable2D& operator= (Renderable2D&&) noexcept;

    //! \brief Submit renderable to the renderer
    //! \details Base class implementation that simply performs the submission.
    //! \param [in] renderer Renderer2D to submit to
    virtual void Submit (Renderer2D* renderer) const;

    //! \brief Set the position of the renderable
    //! \param [in] position vec3 representing the position
    virtual void SetPosition (const rdge::math::vec3& position);

    //! \brief Set the size of the renderable
    //! \param [in] size vec2 representing the size
    virtual void SetSize (const rdge::math::vec2& size);

    //! \brief Set the color of the renderable
    //! \param [in] color Color of the renderable
    //! \param [in] ignore_alpha True will only set the RGB values
    virtual void SetColor (const rdge::gfx::color& color, bool ignore_alpha = true);

    //! \brief Set the texture UV coordinates of the renderable
    //! \param [in] uv UVCoordinates object
    virtual void SetUV (const UVCoordinates& uv);

    //! \brief Set the opacity of the renderable
    //! \details Maps to the alpha value of the color
    //! \param [in] opacity Alpha channel value [0-255]
    virtual void SetOpacity (rdge::uint8 opacity);

    //! \brief Set the opacity of the renderable
    //! \details Maps to the alpha value of the color
    //! \param [in] opacity Alpha channel value [0.0f-1.0f]
    virtual void SetOpacity (float opacity);

    //! \brief Set the Z-Index position value
    //! \param [in] zindex Z-Index value
    virtual void SetZIndex (float zindex);

    //! \brief Scale the size by the provided multiplier
    //! \details Current size is a value of 1.0.
    //! \param [in] scaler Scale value
    //! \throws RDGE::Exception Scaler is not a positive value
    void Scale (float scaler);

    //! \brief Get the position of the renderable object
    //! \returns vec3 of coordinates
    const rdge::math::vec3& Position (void) const noexcept
    {
        return m_position;
    }

    //! \brief Get the size of the renderable object
    //! \returns vec2 of the width and height
    const rdge::math::vec2& Size (void) const noexcept
    {
        return m_size;
    }

    //! \brief Get the color of the renderable object
    //! \returns Fill color
    const rdge::gfx::color& Color (void) const noexcept
    {
        return m_color;
    }

    //! \brief Get the shader unit id of the renderable object
    //! \returns Unit id, or -1 if not available
    rdge::uint32 TextureUnitID (void) const noexcept
    {
        return (m_texture) ? m_texture->unit_id : 0;
        //return m_texture ? static_cast<rdge::int32>(m_texture->UnitID()) : 0;
    }

    //! \brief Get the texture of the renderable object
    //! \returns Shared pointer of the texture
    std::shared_ptr<Texture> Texture (void) const noexcept
    {
        return m_texture;
    }

    //! \brief Get the texture coordinates of the renderable object
    //! \returns Vector of coordinates
    const std::vector<rdge::math::vec2>& UV (void) const noexcept
    {
        return m_uv;
    }

    //! \brief Get the default UV coordinates
    //! \details The default UV coordinates makeup the entire texture.  The
    //!          ordering of the coordinates is important and must follow
    //!          in counter-clockwise starting with the top left coordinate.
    //! \returns UV coordinates container with default values
    static UVCoordinates DefaultUVCoordinates (void)
    {
        return UVCoordinates {
            rdge::math::vec2(0, 1),
            rdge::math::vec2(0, 0),
            rdge::math::vec2(1, 0),
            rdge::math::vec2(1, 1)
        };
    }

protected:
    rdge::math::vec3 m_position;
    rdge::math::vec2 m_size;
    rdge::gfx::color m_color;

    std::shared_ptr<rdge::Texture>      m_texture;
    std::vector<rdge::math::vec2> m_uv;
};

//! \brief UVCoordinates stream output operator
//! \param [in] os Output stream
//! \param [in] uv UV Coordinate vector
//! \returns Output stream
inline std::ostream& operator<< (std::ostream& os, const UVCoordinates& uv)
{
    if (uv.size() != 4)
    {
        return os << "Invalid UVCoordinates";
    }

    std::ostringstream ss;
    ss << "[ " << std::fixed << std::setprecision(5)
       << uv[0] << ", "
       << uv[1] << ", "
       << uv[2] << ", "
       << uv[3] << " ]";

    return os << ss.str();
}

} // namespace gfx
} // namespace rdge
