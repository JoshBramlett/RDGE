#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>

#include "entities/player.hpp"

#include <memory>



// TODO (ongoing thoughts and interface improvements)
// - No clear way to pass update/input events to an entity.
// - Add 'hit box' to the spritesheet.  Similar to the hotspot, but each frame
//   could be have a different size sprite.
// - Idle animation is a single ping pong, but that's not supported so I added
//   each frame in reverse order.
// - The running and walking animations share frames, and since running is just
//   a keyboard modifier it'd make sense not to start the animation over and
//   instead smoothly transition to the next frame


// FIXME bugs
//
// IMPORTANT - The following have not been seen since creating the handler
//             in rdge::KeyboardDirectionalInputHandler.  Leaving here for
//             a reminder in case I see adverse behavior.
//
//     - Very strange bug where he goes off in a direction after all keys are
//       released.  Have not been able to replicate.  I believe the direction
//       has been up, but that may be a coincidence.  Could be that SDL gave me
//       a keyup event and when I queried whether it was pressed it returned
//       true.  If that's the case I'll have to log the events to verify.
//     - Another very strange bug that's only happened a couple times...When
//       first starting the scene the player has a very high velocity.  May be
//       even higher than his run velocity.

class TestScene : public rdge::IScene
{
public:
    TestScene (void);

    void Initialize (void) override;
    void Terminate (void) override;

    void Hibernate (void) override;
    void Activate (void) override;

    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnRender (void) override;

public:
    rdge::OrthographicCamera camera;

    Player player;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::SpriteLayer background;
};
