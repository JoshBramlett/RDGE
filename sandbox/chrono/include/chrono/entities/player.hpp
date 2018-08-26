#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/util.hpp>

#include <chrono/types.hpp>
#include <chrono/entities/iactor.hpp>

class PendingActionCache
{
public:
    void Add (rdge::physics::Contact* contact,
              fixture_user_data* child,
              fixture_user_data* sibling)
    {
        auto handle = m_nodes.reserve();
        auto& node = m_nodes[handle];
        node.next = nullptr;
        node.handle = handle;
        node.contact = contact;
        node.child = child;
        node.sibling = sibling;

        m_actions.push_back(node);
    }

    void Remove (rdge::physics::Contact* contact)
    {
        for (auto& action : m_actions)
        {
            if (action.contact == contact)
            {
                auto handle = action.handle;
                m_actions.remove(action);
                m_nodes.release(handle);
                break;
            }
        }
    }

    size_t Size (void) const noexcept { return m_nodes.size(); }
    bool Empty (void) const noexcept { return m_nodes.empty(); }

private:
    // Pending actions represent sensor contacts that are currently
    // touching, but are not actionable unless invoked by the player.
    // The collision graph callbacks will send the notifications to
    // add/remove values from the list.
    struct pending_action
    {
        pending_action* next;
        rdge::uint32 handle;                // handle to storage
        rdge::physics::Contact* contact;    // contact that's touching
        fixture_user_data* child;           // player fixture data
        fixture_user_data* sibling;         // colliding fixture data
    };

    rdge::intrusive_forward_list<pending_action> m_actions;
    rdge::freelist<pending_action, 32> m_nodes;
};

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

    static Player* Extract (const fixture_user_data* user_data);

private:
    void BeginAttack (void);

public:

    rdge::Direction facing = rdge::Direction::SOUTH;
    rdge::math::vec2 normal; // direction normal

    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hurtbox = nullptr;
    rdge::physics::Fixture* envbox = nullptr;
    rdge::CardinalDirectionArray<fixture_user_data> dir_sensors;

    rdge::physics::RigidBody* sword;
    rdge::physics::Fixture* sword_hitbox;

    PendingActionCache pending_actions;

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
