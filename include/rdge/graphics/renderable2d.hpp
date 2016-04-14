//! \headerfile <rdge/graphics/renderable2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/02/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/color.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Renderable2D
//! \brief Base class for a 2D object to be rendered to the screen
class Renderable2D
{
public:
    //! \brief Renderable2D default empty ctor
    Renderable2D (void) { }

    //! \brief Renderable2D ctor
    //! \param [in] position Location coordinates.  Z-axis is for layering
    //! \param [in] size Width and height of the renderable
    //! \param [in] color Color shared by each vertex
    explicit Renderable2D (
                           const RDGE::Math::vec3& position,
                           const RDGE::Math::vec2& size,
                           const RDGE::Color&      color
                          )
        : m_position(position)
        , m_size(size)
        , m_color(color)
    { }

    //! \brief Renderable2D dtor
    virtual ~Renderable2D (void) { }

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

private:
    RDGE::Math::vec3 m_position;
    RDGE::Math::vec2 m_size;
    RDGE::Color      m_color;
};

} // namespace Graphics
} // namespace RDGE
