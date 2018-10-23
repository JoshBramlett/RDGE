#pragma once

#include <rdge/core.hpp>
#include <rdge/gameobjects/types.hpp>
#include <rdge/math/vec2.hpp>

#include <chrono/entities/iactor.hpp>

//!@{ Forward declarations
namespace rdge {
class Animation;
class Event;
class SpriteLayer;
struct sprite_data;
struct delta_time;
namespace physics {
class CollisionGraph;
class RigidBody;
class Fixture;
} // namespace physics
} // namespace rdge
struct fixture_user_data;
//!@}

class Debutante : public IActor
{
public:
    Debutante (void);

    void Init (const rdge::math::vec2& pos,
               rdge::SpriteLayer& layer,
               rdge::physics::CollisionGraph& graph);

    // Implement when switching between scenes
    //void InitPosition (const rdge::math::vec2& pos, rdge::Direction facing);

    // IActor
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnActionTriggered (const fixture_user_data&) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

public:

    rdge::Direction facing = rdge::Direction::SOUTH;
    rdge::math::vec2 normal;

    rdge::sprite_data* sprite = nullptr;
    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* envbox = nullptr;

private:

    rdge::Animation* m_currentAnimation = nullptr;

    enum StateFlags
    {
        MOVING = 0x0001
    };

    rdge::uint16 m_flags = 0;
};
