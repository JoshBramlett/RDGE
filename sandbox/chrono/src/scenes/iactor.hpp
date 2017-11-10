#pragma once

#include <rdge/math/vec2.hpp>

class IActor
{
public:
    virtual ~IActor (void) = default;

    virtual void OnMeleeAttack (float damage, const rdge::math::vec2& pos) = 0;
    virtual rdge::math::vec2 GetWorldCenter (void) const noexcept = 0;
};
