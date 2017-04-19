#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>

#include "entities/player.hpp"
#include "entities/duck.hpp"

#include <memory>

// TODO Immediate
// - Create an enemy entity.
//   The reason for doing so is because I want to create a collision system
//   based on the supposition that entities need to react to what type of object
//   it's colliding with.  Yes, the physics part can be dumb, but the callback
//   handler should be able to do more.  e.g. A player colliding with an enemy
//   is different than a player colliding with a wall.
//
//   Perhaps have a struct that contains a shape, and some values that denote
//   how the other colliding shape can react?  e.g. a universal damage value,
//   so a player can look to that to determine how hard the enemy hits, but a
//   wall wouldn't care?
//
//   Would it be best to have all collidables provide their displacement, then
//   have the collision engine provide callbacks to resolve, then continue
//   with the update phase?  Only finally continuing once everything has resolved?
//
// Ideas from Box2D to implement
//   - Define MKS system (meters, kilograms, seconds)
//     - Values should be constant, immutable, and well known
//   - Create world system (kinda like a renderer) that holds all the collidables
//     and performs the simulation for a timestep
//     - Could implement the factory pattern to store data efficiently
//     - Could do all the displacement which are then used by the renderer
//     - Callback mechanism for handling collisions
//   - Shapes
//     - Circle (with debug drawing)
//     - Polygon (with convex hull algorithm)
//   - Broad phase using a naive linear list until a dynamic tree is needed
//   - Body with body type
//   - Fixtures with
//     - User data
//     - Filtering flags
//     - Sensor
//
// Box2D Manual notes:
//
// 4. Collision System
//   Includes shapes and functions that operate on them.  Includes broad phase
//   using a dynamic tree.
//
//   - Shapes
//     - Circle
//     - Polygon
//     - Edge (line segment, cannot collide with one another)
//     - Chain (chain of edges)
//   - Unary queries (single shape)
//     - Shape/Point test
//     - Shape/RayCast test
//   - Binary queries (two shapes)
//     - Overlap (intersects_with)
//     - Manifold generation
//     - Distance
//     - Time of Impact
//   - Dynamic Tree
//     - Organizes location efficiently so queries can be done by region
//   - Broad phase
//     - Uses the dynamic tree to minimize narrow phase
//
// 5. Dynamics module
//   Sits on top of the collision system.  Includes:
//
//   - Fixture
//   - Rigid Body
//   - Contact
//   - Joint
//   - World
//   - Listener
//
// 6. Bodies
//   Have position and velocity.  Forces, torque, and impulses can be applied.
//   They carry fixtures.
//
//   - Body types
//     - Static
//       - Has no velocity and infinite mass, only collides with dynamic
//       - Useful for trees, houses, etc.
//     - Kinetic
//       - Has velocity but does not respond to forces.  Only collides with dynamic
//       - Useful for forcefields or noise detection (for stealth games)
//     - Dynamic
//       - Full simulation, collides with everything
//
//  -6.1 Body Definition
//     Like a profile object, it contains data to init a body.  Can be reused.
//
//     - Position and angle
//       - Body Origin
//         - Fixtures and joints are attached relative to the body origin
//       - Center of mass
//         - Made up of the attached fixtures and joints
//     - Damping
//       - Used to reduce velocity of bodies
//       - Different than friction b/c damping occurs without contact
//     - Gravity Scale
//     - Sleeping
//       - If a body comes to a resting state it'll be marked to sleep so as to
//         stop simulation on it
//       - If an awake body collides it'll wake up
//     - Fixed rotation
//       - Set the body to not rotate
//     - Bullets
//       - Labeling a body as a bullet means it's expected to have a large
//         displacement during a timestep, so the simulation can accommodate
//     - User data (opaque pointer)
//
//  -6.2 Body Factory
//     Allows for efficient allocation with cache locality
//
//  -6.3 Using a body
//
//     - Mass
//       - Has mass (scalar), center of mass (2-vector), and rotational inertia (scalar)
//       - Mass data is generally generated automatically when attaching fixtures
//     - Position and velocity
//       - Can be set, but simulation should set it automatically and can be
//         read for rendering coordinates
//
// 7. Fixtures
//   Attached to a body.  Contains:
//
//   - Single shape
//   - Broad-phase proxies ???
//   - Density, friction, and restitution
//   - Collision filtering flags
//   - Back pointer to the parent body
//   - User data
//   - Sensor flag
//
//  -7.1 Fixture creation
//     - Density
//       - Used to calculate parent bodies mass
//     - Friction
//       - How two fixtures slide along one another
//     - Restitution
//       - Bounciness
//     - Filtering
//       - Signify which fixtures can collide
//       - Category
//         - Categorize the fixture
//       - Mask
//         - Specify which categories can collide
//     - Sensors
//       - For determining whether there is a collision, but shuldn't have a response
//
// 8. Joints
//   Constrain bodies together (e.g. ragdoll)
//
//   - NOTE Some pretty cool stuff here, but not sure how useful it is for my use case
//
// 9. Contacts
//   Basically the manifold.  Has uses with listeners for callbacks signifying two
//   fixtures have collided.
//
// 10. World
//   Singleton that contains the bodies and performs the simulation.  Queries can be
//   done speciyfing a RayCast or AABB that'll return all the bodies that collide.

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

// FIXME Bugs
// - Background CreateSpriteChain has tearing in between tiles
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
    Duck duck;
    struct wall_boundary {
        rdge::math::aabb left;
        rdge::math::aabb top;
        rdge::math::aabb right;
        rdge::math::aabb bottom;
    } walls;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::SpriteLayer background;
};
