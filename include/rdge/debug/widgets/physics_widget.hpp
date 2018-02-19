//! \headerfile <rdge/debug/widgets/physics_widget.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/12/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/debug/widgets/iwidget.hpp>
#include <rdge/graphics/color.hpp>

//!@{ Forward declarations
namespace rdge {
namespace physics {
class CollisionGraph;
}
}
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

//! \class PhysicsWidget
//! \brief ImGui Wiget for runtime physics control
class PhysicsWidget : public IWidget
{
public:
    ~PhysicsWidget (void) noexcept = default;

    void UpdateWidget (void) override;
    void OnWidgetCustomRender (void) override;

    physics::CollisionGraph* graph = nullptr;
    float scale = 1.f;
};

} // namespace debug
} // namespace rdge
