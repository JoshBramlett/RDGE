#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>

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

enum class PlayerStateType : rdge::uint32
{
    Idle,
    Walking,
    Running
};

enum class PlayerFacing : rdge::uint32
{
    Front,
    Right,
    Back,
    Left
};

class Player;

struct player_state
{
    virtual ~player_state (void) = default;

    virtual const rdge::texture_part& GetFrame (const Player& player,
                                                rdge::uint32 ticks) = 0;

    std::vector<rdge::Animation> animations;
};

struct idle_state : public player_state
{
    virtual const rdge::texture_part& GetFrame (const Player& player,
                                                rdge::uint32 ticks) override;
    rdge::uint32 offset = 0;
};

struct walking_state : public player_state
{
    virtual const rdge::texture_part& GetFrame (const Player& player,
                                                rdge::uint32 ticks) override;
};

struct running_state : public player_state
{
    virtual const rdge::texture_part& GetFrame (const Player& player,
                                                rdge::uint32 ticks) override;
};





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



// TODO Move acceleration_displacement and velocity_displacement to the engine
struct acceleration_displacement
{
    // TODO Friction is not accounted for, so the player will go on forever
    //      HMH talks of ordinary differential equations
    void calculate (rdge::math::vec2& unit_vector, const rdge::delta_time& dt, int index = 0)
    {
        rdge::math::vec2 acceleration = unit_vector;
        acceleration *= unit_length * base_velocities[index];

        // emulates friction - but acceleration constant would need to be jacked up
        //acceleration += velocity * -5.5f;

        unit_vector = ((.5f * acceleration) * (dt.seconds * dt.seconds)) +
                      (velocity * dt.seconds);
        velocity = (acceleration * dt.seconds) + velocity;
    }

    rdge::math::vec2 velocity;
    float unit_length = 0.f;
    std::vector<float> base_velocities;
};

struct velocity_displacement
{
    void calculate (rdge::math::vec2& unit_vector, const rdge::delta_time& dt, int index = 0)
    {
        unit_vector *= unit_length * dt.seconds * base_velocities[index];
    }

    float unit_length = 0.f;
    std::vector<float> base_velocities;
};

class Player
{
public:
    Player (void);

    void OnEvent (const rdge::Event& event);
    void OnUpdate (const rdge::delta_time& dt);

public:
    PlayerStateType state_type = PlayerStateType::Idle;
    PlayerFacing facing = PlayerFacing::Front;

    rdge::KeyboardDirectionalInputHandler dir_handler;
    bool run_pressed = false;
    velocity_displacement vdisp;
    acceleration_displacement adisp;

    std::vector<std::unique_ptr<player_state>> states;
    player_state* current_state = nullptr;
    bool current_state_changed = false;

    std::shared_ptr<rdge::Sprite> sprite;
};




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
