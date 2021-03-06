//! \headerfile <rdge/debug/widgets/camera_widget.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/10/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/debug/widgets/iwidget.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \class CameraWidget
//! \brief ImGui Wiget for runtime camera control
class CameraWidget : public IWidget
{
public:
    ~CameraWidget (void) noexcept = default;

    void UpdateWidget (void) override;
    void OnWidgetCustomRender (void) override;
};

} // namespace debug
} // namespace rdge
