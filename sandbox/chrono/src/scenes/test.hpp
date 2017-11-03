#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>

#include "entities/player.hpp"
#include "entities/duck.hpp"
#include "entities/dove.hpp"

#include <memory>

// TODO Immediate
//
// ** GRAPHICS **
// 1. Create an Entity/Actor interface or POD struct?
//   - struct would include a Sprite and a RigidBody.
//   - interface would include pure virtual methods for getting body/sprite data
//   - Both should be raw pointers to block allocated objects to have better
//     cache locality.
//
// 2. Refactor how Sprite is used across code base (from shared_ptr to raw)
//
// 3. Refactor SpriteBatch to use a custom allocator

// TODO (ongoing thoughts and interface improvements)
// - No clear way to pass update/input events to an entity.
// - Idle animation is a single ping pong, but that's not supported so I added
//   each frame in reverse order.

// TODO Polish
// - The running and walking animations share frames, and since running is just
//   a keyboard modifier it'd make sense not to start the animation over and
//   instead smoothly transition to the next frame

class TestScene : public rdge::IScene, public rdge::physics::GraphListener
{
public:
    TestScene (void);
    ~TestScene (void) noexcept = default;

    // scene transitions
    void Initialize (void) override;
    void Terminate (void) override;
    void Hibernate (void) override;
    void Activate (void) override;

    // scene game loop events
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnRender (void) override;

    // physics events
    void OnContactStart (rdge::physics::Contact*) override;
    void OnContactEnd (rdge::physics::Contact*) override;
    void OnPreSolve (rdge::physics::Contact*, const rdge::physics::collision_manifold&) override;
    void OnPostSolve (rdge::physics::Contact*) override;
    void OnDestroyed (rdge::physics::Fixture*) override;

public:
    rdge::OrthographicCamera camera;
    rdge::physics::CollisionGraph collision_graph;

    Player player;
    Duck duck;
    Duck duck2;
    Dove dove;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::TilemapBatch background;
    rdge::SpriteLayer entities;
};
