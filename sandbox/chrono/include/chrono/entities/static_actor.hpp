#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/types.hpp>

#include <chrono/entities/iactor.hpp>
#include <chrono/util/tilemap_helpers.hpp>

//!@{ Forward declarations
namespace rdge {
class SpriteSheet;
class SpriteLayer;
class Event;
struct delta_time;
struct sprite_data;

namespace physics {
class CollisionGraph;
class RigidBody;
class Fixture;
} // namespace physics

namespace tilemap {
class Object;
} // namespace tilemap

} // namespace rdge
//!@}

class StaticActor : public IActor
{
public:
    static constexpr size_t MAX_FIXTURES = 8;

    StaticActor (const rdge::tilemap::Object& def,
                 const rdge::SpriteSheet& sheet,
                 rdge::SpriteLayer& layer,
                 rdge::physics::CollisionGraph& graph);

    // IActor
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnMeleeAttack (float damage, const rdge::math::vec2& pos) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;
    bool IsActionable (void) const noexcept override;
    rdge::ActionType GetActionType (void) const noexcept override;

public:
    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;

    size_t num_fixtures = 0;
    rdge::physics::Fixture* fixtures[MAX_FIXTURES];

    action_trigger_data trigger;

private:
    rdge::uint32 m_actorId = 0;
    rdge::ActionType m_actionType = rdge::ActionType::NONE;
};
