#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include "cardinal_direction_animation.hpp"

#include <vector>
#include <memory>

class TestScene;

class Duck
{
public:
    Duck (TestScene* parent);

    void OnUpdate (const rdge::delta_time& dt);

    void InitPhysics (rdge::physics::CollisionGraph& graph, const rdge::math::vec2& pos);

public:
    rdge::Animation* current_animation = nullptr;
    CardinalDirectionAnimation cd_anim_walk;

    rdge::Direction facing = rdge::Direction::NONE;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body;

    TestScene* m_parent;
};
