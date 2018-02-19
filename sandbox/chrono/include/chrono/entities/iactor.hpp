#pragma once

#include <rdge/math/vec2.hpp>

//!@{ Forward declarations
namespace rdge {
class Event;
struct delta_time;
} // namespace rdge
//!@}

class IActor
{
public:
    virtual ~IActor (void) = default;

    virtual void OnEvent (const rdge::Event& event) = 0;
    virtual void OnUpdate (const rdge::delta_time& dt) = 0;

    virtual void OnMeleeAttack (float damage, const rdge::math::vec2& pos) = 0;
    virtual rdge::math::vec2 GetWorldCenter (void) const noexcept = 0;
};
