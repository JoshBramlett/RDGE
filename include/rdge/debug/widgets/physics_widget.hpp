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

    void Update (void) override;
    void OnCustomRender (void) override;

    physics::CollisionGraph* graph = nullptr;
    float scale = 1.f;

    //! \brief Rendering colors
    struct draw_colors
    {
        color not_simulating = color(127, 127, 76);
        color static_body    = color(127, 230, 127);
        color kinematic_body = color(127, 127, 230);
        color sleeping_body  = color(152, 152, 152);
        color dynamic_body   = color(230, 178, 178);
        color proxy_aabb     = color(230, 76, 230);
        color center_of_mass = color::WHITE;
        color joints         = color::CYAN;
    } colors;
};

} // namespace debug
} // namespace rdge
