//! \headerfile <rdge/debug/widgets/memory_widget.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 02/21/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/debug/widgets/iwidget.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \class MemoryWidget
//! \brief ImGui Wiget for memory tracking
class MemoryWidget : public IWidget
{
public:
    ~MemoryWidget (void) noexcept = default;

    void UpdateWidget (void) override;
    void OnWidgetCustomRender (void) override;
};

} // namespace debug
} // namespace rdge
