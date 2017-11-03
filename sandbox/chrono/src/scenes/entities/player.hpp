#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include <vector>
#include <memory>

class Player
{
public:
    Player (void);

    void InitPhysics (rdge::physics::CollisionGraph& graph, const rdge::math::vec2& pos);
    void OnEvent (const rdge::Event& event);
    void OnUpdate (const rdge::delta_time& dt);

    rdge::math::vec2 GetWorldCenter (void) const noexcept;

public:

    rdge::Direction facing = rdge::Direction::SOUTH;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hitbox = nullptr;
    rdge::CardinalDirectionArray<rdge::physics::Fixture*> dir_sensors;

    rdge::physics::RigidBody* sword;
    rdge::physics::Fixture* sword_hitbox;

private:
    // input handling
    rdge::KeyboardDirectionalInputHandler m_handler;
    rdge::math::vec2 m_direction;
    rdge::Animation* m_currentAnimation = nullptr;

    enum StateFlags
    {
        RUN_BUTTON_PRESSED    = 0x0001,
        ATTACK_BUTTON_PRESSED = 0x0002,
        ATTACKING             = 0x0004
    };

    rdge::uint16 m_flags = 0;
};
