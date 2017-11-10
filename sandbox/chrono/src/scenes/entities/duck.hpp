#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include "../iactor.hpp"

#include <memory>

class TestScene;

class Duck : public IActor
{
public:
    Duck (TestScene* parent);

    void InitPhysics (rdge::physics::CollisionGraph& graph, const rdge::math::vec2& pos);
    void OnUpdate (const rdge::delta_time& dt);

    // IActor
    void OnMeleeAttack (float damage, const rdge::math::vec2& pos) override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

public:

    rdge::Direction facing = rdge::Direction::SOUTH;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hitbox = nullptr;

    float kb_impulse = 45.f;
    float kb_damping = 35.f;

private:

    // TODO remove
    TestScene* m_parent;
    rdge::uint32 m_delay = 0;

    rdge::Animation* m_currentAnimation = nullptr;

    enum StateFlags
    {
        ATTACKED = 0x0001,
    };

    rdge::uint16 m_flags = 0;
};
