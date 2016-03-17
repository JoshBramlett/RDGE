//! \headerfile <rdge/viewport.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 02/04/2016
//! \bug

#pragma once

#include <string>
#include <memory>

#include <rdge/types.hpp>
#include <rdge/graphics/rect.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \class Viewport
//! \brief Relative portion of the screen for drawing
//! \details Should be less than or equal to the window size
class Viewport
{
public:
    //! \brief Viewport ctor
    //! \details Provides drawable dimensions
    //! \param [in] dimensions Rectangle representing the drawing area
    explicit Viewport (const RDGE::Graphics::Rect& dimensions);

    //! \brief Viewport dtor
    ~Viewport (void) { }

    //! \brief Viewport Copy ctor
    //! \details Default-copyable
    Viewport (const Viewport&) = default;

    //! \brief Viewport Move ctor
    //! \details Default-movable
    Viewport (Viewport&& rhs) = default;

    //! \brief Viewport Copy Assignment Operator
    //! \details Default-copyable
    Viewport& operator= (const Viewport&) = default;

    //! \brief Viewport Move Assignment Operator
    //! \details Default-movable
    Viewport& operator= (Viewport&& rhs) = default;

    //! \brief Get the viewport dimensions
    //! \returns Rect represeting the viewport dimensions
    RDGE::Graphics::Rect Dimensions (void) const
    {
        return m_dimensions;
    }

private:
    RDGE::Graphics::Rect m_dimensions;
};

} // namespace RDGE
