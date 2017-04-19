#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>

#include <array>

class CardinalDirectionAnimation
{
public:
    rdge::Animation& operator[] (rdge::Direction dir)
    {
        return animations[rdge::math::lsb(rdge::to_underlying(dir)) - 1];
    }

    std::array<rdge::Animation, 4> animations;
};
