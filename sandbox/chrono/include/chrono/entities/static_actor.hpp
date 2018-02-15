#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <chrono/entities/iactor.hpp>

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
    StaticActor (const rdge::tilemap::Object& def,
                 const rdge::SpriteSheet& sheet,
                 rdge::SpriteLayer& layer,
                 rdge::physics::CollisionGraph& graph);

    // IActor
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnMeleeAttack (float damage, const rdge::math::vec2& pos) override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

public:
    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* hitbox = nullptr;
};
