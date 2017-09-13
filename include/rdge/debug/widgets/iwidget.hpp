//! \headerfile <rdge/debug/widgets/iwidget.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/12/2017

#pragma once

#include <rdge/core.hpp>

#include <imgui/imgui.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \class IWidget
//! \brief Interface for ImGui Wigets
class IWidget
{
public:
    //! \brief IWidget dtor
    virtual ~IWidget (void) noexcept = default;

    //! \brief Update widget state, potentially rendering the widget on screen
    virtual void Update (void) = 0;

    //! \brief Allows the widget to do any custom rendering
    //! \details Called during the OnRender phase, the widget can perform any custom
    //!          debug drawing.  For example, the physics system would render the
    //!          the fixture wireframes here.
    virtual void OnCustomRender (void) = 0;
};

} // namespace debug
} // namespace rdge
