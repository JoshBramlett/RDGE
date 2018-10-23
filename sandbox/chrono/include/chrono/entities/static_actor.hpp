#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/gameobjects/types.hpp>

#include <chrono/entities/iactor.hpp>
#include <chrono/types.hpp>

#include <vector>

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
    void OnEvent (const rdge::Event&) override;
    void OnUpdate (const rdge::delta_time&) override;
    void OnActionTriggered (const fixture_user_data&) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

public:
    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;

    //!@{ Fixtures (collision / triggers)
    std::vector<fixture_user_data> collidables;
    std::vector<fixture_user_data> triggers;
    //!@}

private:
    rdge::uint32 m_actorId = 0;
};
