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
    static constexpr float MARGIN = 10.f; //!< Default widget margin

    //! \brief IScene dtor
    virtual ~IWidget (void) noexcept = default;

    //! \brief Renders the widget on screen
    virtual void Draw (void) = 0;
};

} // namespace debug
} // namespace rdge
