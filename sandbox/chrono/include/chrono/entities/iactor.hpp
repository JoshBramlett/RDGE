#pragma once

#include <rdge/math/vec2.hpp>

//!@{ Forward declarations
namespace rdge {
class Event;
struct delta_time;
enum class ActionType;
} // namespace rdge
//!@}

// - Scene transition
//      - outgoing
//          - scene_stack_action
//          - scene_id to transition to
//          - entity_id (for spawn point)
//          - [out] animation
//          - [in] animation

// Door
// - Fixture sensor
//      - invokable
//      - facing direction




// Different types of actors
// 1) Static
//    - Tile collision
// 2) Container
//    - Fixtures
//      - Tile collision
//      - Tile sensor
//    - Actionable
//      - Inventory
//    - Properties
//      - Unique ObjectID (for serialization)
//      - ActorID?
//      - Open/Closed
//      - Contained items
//      - IsSafe (whether items can be randomly added/removed)
// 3) Sign
//    - Fixtures
//      - Tile collision
//      - Tile sensor
//    - Actionable
//      - Dialog
//    - Properties
//      - Unique ObjectID?
//      - ActorID (for dialog lookup)
// 3) NPC
//    - Tile collision
//    - Tile sensor
//    - Actionable
//    - Dialog
//    - Inventory
//
//
// GetActorID()
// GetObjectID()
// GetWorldCenter()
//
// IsActionable()
// InvokeAction()



class IActor
{
public:
    virtual ~IActor (void) = default;

    virtual void OnEvent (const rdge::Event& event) = 0;
    virtual void OnUpdate (const rdge::delta_time& dt) = 0;

    virtual rdge::uint32 GetActorId (void) const noexcept = 0;
    virtual rdge::math::vec2 GetWorldCenter (void) const noexcept = 0;
    virtual bool IsActionable (void) const noexcept = 0;
    virtual rdge::ActionType GetActionType (void) const noexcept = 0;
};

#if 0
class Container : public IActor
{
public:
    explicit Container (const nlohmann::json&);
    ~Container (void) = default;

    void OnEvent (const rdge::Event& event) = 0;
    void OnUpdate (const rdge::delta_time& dt) = 0;

    rdge::uint32 GetActorId (void) const noexcept = 0;
    rdge::math::vec2 GetWorldCenter (void) const noexcept = 0;
    bool IsActionable (void) const noexcept = 0;
    rdge::ActionType GetActionType (void) const noexcept = 0;

    rdge::sprite_data* sprite = nullptr;

    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* collision = nullptr;
    rdge::physics::Fixture* sensor = nullptr;
    fixture_user_data sensor_user_data;

private:
    enum class ContainerState
    {
        CLOSED  = 0,
        OPEN    = 1,
        OPENING = 2
    } m_state;

    rdge::Animation opening_animation;

    enum StateFlags
    {
        RUN_BUTTON_PRESSED    = 0x0001,
        ATTACK_BUTTON_PRESSED = 0x0002,
        INPUT_LOCKED          = 0x0004,
        ATTACKING             = 0x0010
    };

    rdge::uint16 m_flags = 0;
};
#endif
