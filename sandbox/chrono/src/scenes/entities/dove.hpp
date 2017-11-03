#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include <vector>
#include <memory>

class TestScene;

class Dove
{
public:
    Dove (void);

    void InitPhysics (rdge::physics::CollisionGraph& graph, const rdge::math::vec2& pos);

    void Spawn (rdge::math::vec2 pos);
    void Disable (void);

    void OnUpdate (const rdge::delta_time& dt);

    rdge::math::vec2 GetWorldCenter (void) const noexcept;

public:
    // TODO bird only flies one way
    bool is_flying = false;
    rdge::Direction facing = rdge::Direction::LEFT;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body = nullptr;

private:
    rdge::Animation* m_currentAnimation = nullptr;
};
