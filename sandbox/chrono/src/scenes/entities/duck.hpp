#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics/motion.hpp>

#include "cardinal_direction_animation.hpp"

#include <vector>
#include <memory>

class Duck
{
public:
    Duck (const rdge::math::vec3& position);

    void OnUpdate (const rdge::delta_time& dt);

public:
    rdge::Animation* current_animation = nullptr;
    CardinalDirectionAnimation cd_anim_walk;

    rdge::Direction facing = rdge::Direction::NONE;

    std::shared_ptr<rdge::Sprite> sprite;
};
