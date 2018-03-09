//! \headerfile <rdge/debug/widgets/graphics_widget.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 03/08/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/debug/widgets/iwidget.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \class GraphicsWidget
//! \brief ImGui Wiget for runtime graphics control
class GraphicsWidget : public IWidget
{
public:
    ~GraphicsWidget (void) noexcept = default;

    void UpdateWidget (void) override;
    void OnWidgetCustomRender (void) override;
};

} // namespace debug
} // namespace rdge
