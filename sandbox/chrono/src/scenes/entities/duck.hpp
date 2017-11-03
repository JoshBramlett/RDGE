#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include <memory>

class TestScene;

class Duck
{
public:
    Duck (TestScene* parent);

    void InitPhysics (rdge::physics::CollisionGraph& graph, const rdge::math::vec2& pos);
    void OnUpdate (const rdge::delta_time& dt);

    rdge::math::vec2 GetWorldCenter (void) const noexcept;

public:

    rdge::Direction facing = rdge::Direction::SOUTH;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hitbox = nullptr;

private:

    // TODO remove
    TestScene* m_parent;

    rdge::Animation* m_currentAnimation = nullptr;
};
