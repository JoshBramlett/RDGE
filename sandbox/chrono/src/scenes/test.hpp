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

#include <memory>

// TODO Immediate
//
// ** ISSUES/QUESTIONS **
// 1. How should the entity be aware of the scene's pixels to meters conversion?
//   a) Should the entity not be aware of it at all?
//   b) Pass the values on every render event so they can update their sprite verts?
//   c) Have every entity cache the conversion during construction?
//
//   The problem with creating a cardinal interface is there is no one size fits all
//   solution.  Some entities have both a sprite and body.  Others may only have a
//   sprite, or only have a body.
//
//   This obviously only applies to entities that are part of the physics simulation.
//   It doesn't make sense to force conversion for entities that aren't.
//
// 2. Figure out how I want to render a tiled background.  Fixes "padding" issue.
//   a) Implement texture wrapping for sprite chains
//   b) Render sprite chains by themselves with their own custom IBO
//   c) Don't.  Do like HMH which is layer a bunch of semi-transparent textures
//
//
// ** GRAPHICS **
// 1. Create an Entity/Actor interface or POD struct?
//   - struct would include a Sprite and a RigidBody.
//   - interface would include pure virtual methods for getting body/sprite data
//   - Both should be raw pointers to block allocated objects to have better
//     cache locality.
//
// 2. Each scene must hold ratio for meters to pixels
//   - The physics simulation should be responsible for updating the position,
//     which will be sent to the renderer.
//   - Each scene should hold the ratio for meters to pixels, and either pass
//     that to the entities so they can do the conversion, or iterate through
//     every renderable and perform the conversion.
//
// 3. Refactor how Sprite is used across code base (from shared_ptr to raw)
//
// 4. Refactor SpriteBatch to use a custom allocator

// TODO (ongoing thoughts and interface improvements)
// - No clear way to pass update/input events to an entity.
// - Add 'hit box' to the spritesheet.  Similar to the hotspot, but each frame
//   could be have a different size sprite.
// - Idle animation is a single ping pong, but that's not supported so I added
//   each frame in reverse order.

// TODO Polish
// - The running and walking animations share frames, and since running is just
//   a keyboard modifier it'd make sense not to start the animation over and
//   instead smoothly transition to the next frame

class TestScene : public rdge::IScene
{
public:
    static constexpr float PIXELS_PER_METER = 64.f;
    static constexpr float INV_PIXELS_PER_METER = 1.f / 64.f; // 0.015625

    TestScene (void);

    void Initialize (void) override;
    void Terminate (void) override;

    void Hibernate (void) override;
    void Activate (void) override;

    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnRender (void) override;

    void Debug_OnWidgetUpdate (rdge::debug::scene_widget_settings&) override;

public:
    rdge::OrthographicCamera camera;
    rdge::physics::CollisionGraph collision_graph;

    Player player;
    Duck duck;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::SpriteLayer background;
    rdge::SpriteLayer entities;
};
