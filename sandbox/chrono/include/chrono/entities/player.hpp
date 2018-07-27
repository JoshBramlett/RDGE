#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include <chrono/entities/iactor.hpp>

class Player : public IActor
{
public:
    Player (void);

    void Init (const rdge::math::vec2& pos,
               rdge::SpriteLayer& layer,
               rdge::physics::CollisionGraph& graph);

    void InitPosition (const rdge::math::vec2& pos, rdge::Direction facing);


    bool IsAttacking (void) const noexcept { return m_flags & ATTACKING; }

    // IActor
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnMeleeAttack (float damage, const rdge::math::vec2& pos) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;
    bool IsActionable (void) const noexcept override;
    rdge::ActionType GetActionType (void) const noexcept override;

private:

    void BeginAttack (void);

public:

    rdge::Direction facing = rdge::Direction::SOUTH;
    rdge::math::vec2 normal; // direction normal

    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hurtbox = nullptr;
    rdge::physics::Fixture* envbox = nullptr;
    rdge::CardinalDirectionArray<rdge::physics::Fixture*> dir_sensors;

    rdge::physics::RigidBody* sword;
    rdge::physics::Fixture* sword_hitbox;

private:
    // input handling
    rdge::KeyboardDirectionalInputHandler m_handler;
    rdge::Animation* m_currentAnimation = nullptr;

    float m_lockedVelocity = 0.f;


    enum StateFlags
    {
        RUN_BUTTON_PRESSED    = 0x0001,
        ATTACK_BUTTON_PRESSED = 0x0002,
        INPUT_LOCKED          = 0x0004,
        ATTACKING             = 0x0010
    };

    rdge::uint16 m_flags = 0;
};
