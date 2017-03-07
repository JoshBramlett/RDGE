#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics.hpp>

#include <memory>

// TODO (ongoing thoughts and interface improvements)
// - No clear way to pass update/input events to an entity.
// - Add 'animation' to the spritesheet, so it doesn't have to be defined in code
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

// Persistant frame input handling
// SDL keyboard event handling has some kinda funny behavior.  Only the last
// key pressed is sent as an event, which is problematic when having to handle
// if multiple keys are pressed simultaneously.  There are two options to handle
// the behavior -
//     a) Ignore the event data and do IsKeyPressed for every OnEvent
//     b) Create a persistant cache (which is this struct)
// The main benefit of (b) is performance with not having to query every key state
// every time there's a keyboard event.
//
// TODO Rename this - maybe need to make it more universal (or even add parts
//      of it to the engine)
//
// FIXME bugs
//     - Redo facing logic.  Fails saving the correct state when more than two
//       keys are pressed and released.  Also, a single key press release will
//       revert to the previous direction.  e.g. facing left - right pressed and
//       released.  The correct direction should be right, but it reverts to the
//       previous (left).
//     - Very strange bug where he goes off in a direction after all keys are
//       released.  Have not been able to replicate.  I believe the direction
//       has been up, but that may be a coincidence.  Could be that SDL gave me
//       a keyup event and when I queried whether it was pressed it returned
//       true.  If that's the case I'll have to log the events to verify.
struct input_handler
{
    // TODO: Potentially a stop-gap solution:
    //       The displacement vector is based upon a unit circle in order to
    //       normalize the magnitude regardless of the direction.  The value
    //       represents the x and y coordinates when the vector has an angle
    //       of 45 degrees.
    //
    //       Value computes from [sin(45deg), cos(45deg)], or using the
    //       pythagorean theorem where x and y are equal and the diagonal
    //       has a value of 1 (x^2 + x^2 = d^2), solving for x evaluates to
    //       x = sqrt(1/2).
    static constexpr float DIAGONAL_DISPLACEMENT = 0.707106781187f;
    static constexpr float WALK_VELOCITY = 10.f;
    static constexpr float RUN_VELOCITY = 20.f;

    rdge::KeyCode key_move_up    = rdge::KeyCode::W;
    rdge::KeyCode key_move_right = rdge::KeyCode::D;
    rdge::KeyCode key_move_down  = rdge::KeyCode::S;
    rdge::KeyCode key_move_left  = rdge::KeyCode::A;
    rdge::KeyCode key_run        = rdge::KeyCode::J;

    // all flags persist from frame to frame - these correspond to key states.
    // (i.e. set to true when pressed, false when not pressed)
    // TODO Could (should?) be a bitset
    bool walk_up_pressed    = false;
    bool walk_left_pressed  = false;
    bool walk_down_pressed  = false;
    bool walk_right_pressed = false;
    bool run_pressed        = false;

    // Set to true when a flag has changed.  Only for the current frame, so it
    // should be reset to false for the beginning of the next frame.  The calculate
    // function does that, so make sure to call that every frame.
    bool is_dirty = false;

    rdge::math::vec2 uvec; // Coordinates of a unit circle to offset the position
    rdge::math::vec2 velocity;

    PlayerFacing facing = PlayerFacing::Front;

    void update_facing (PlayerFacing f, bool new_direction)
    {
        // since multiple keyboard keys can be pressed simultaneously, special
        // logic is required to determine the correct facing direction.
        //
        // Logic is basically:
        //     - If a new key is pressed, that's the direction
        //     - Is a key is released:
        //         - Matches the last direction, set to previous
        //         - Doesn't match last direction, do nothing, but reset previous
        static PlayerFacing previous = PlayerFacing::Front;

        if (new_direction)
        {
            previous = facing;
            facing = f;
        }
        else if (facing == f)
        {
            facing = previous;
        }
        else
        {
            previous = facing;
        }
    }

    void set_key_state(rdge::KeyCode key, bool is_pressed)
    {
        if (key == key_move_up)
        {
            walk_up_pressed = is_pressed;
            is_dirty = true;

            update_facing(PlayerFacing::Back, is_pressed);
        }
        else if (key == key_move_left)
        {
            walk_left_pressed = is_pressed;
            is_dirty = true;

            update_facing(PlayerFacing::Left, is_pressed);
        }
        else if (key == key_move_down)
        {
            walk_down_pressed = is_pressed;
            is_dirty = true;

            update_facing(PlayerFacing::Front, is_pressed);
        }
        else if (key == key_move_right)
        {
            walk_right_pressed = is_pressed;
            is_dirty = true;

            update_facing(PlayerFacing::Right, is_pressed);
        }
        else if (key == key_run)
        {
            run_pressed = is_pressed;
            is_dirty = true;
        }
    }

    bool is_moving (void) const noexcept
    {
        return (walk_up_pressed != walk_down_pressed) ||
               (walk_left_pressed != walk_right_pressed);
    }

    bool is_walking (void) const noexcept
    {
        return is_moving() && !run_pressed;
    }

    bool is_running (void) const noexcept
    {
        return is_moving() && run_pressed;
    }

    void calculate (const rdge::delta_time& dt)
    {
        if (!is_dirty)
        {
            // nothing changed since last frame.  Displacement vector can be
            // reused on next draw
            return;
        }

        uvec = { 0.f, 0.f }; // reset every frame (if dirty)

        if (!is_moving())
        {
            return;
        }

        uvec.x += walk_left_pressed ? -1.f : 0.f;
        uvec.x += walk_right_pressed ? 1.f : 0.f;
        uvec.y += walk_up_pressed ? 1.f : 0.f;
        uvec.y += walk_down_pressed ? -1.f : 0.f;

        if (uvec.x != 0.f && uvec.y != 0.f)
        {
            uvec *= DIAGONAL_DISPLACEMENT;
        }

        uvec *= is_running() ? RUN_VELOCITY : WALK_VELOCITY; // base velocity (m/s)
        uvec *= 64.f;                                        // unit distance (meters)
        uvec *= dt.seconds;                                  // delta time

        is_dirty = false;
    }

    // TODO Friction is not accounted for, so the player will go on forever
    //      HMH talks of ordinary differential equations
    void calculate_with_acceleration (const rdge::delta_time& dt)
    {
        rdge::math::vec2 acceleration = { 0.f, 0.f };

        acceleration.x += walk_left_pressed ? -1.f : 0.f;
        acceleration.x += walk_right_pressed ? 1.f : 0.f;
        acceleration.y += walk_up_pressed ? 1.f : 0.f;
        acceleration.y += walk_down_pressed ? -1.f : 0.f;

        if (acceleration.x != 0.f && acceleration.y != 0.f)
        {
            acceleration *= DIAGONAL_DISPLACEMENT;
        }

        acceleration *= is_running() ? RUN_VELOCITY : WALK_VELOCITY;
        acceleration *= 64.f;

        // emulates friction - but acceleration constant would need to be jacked up
        //acceleration += velocity * -5.5f;

        uvec = ((.5f * acceleration) * (dt.seconds * dt.seconds)) +
               (velocity * dt.seconds);
        velocity = (acceleration * dt.seconds) + velocity;

        is_dirty = false;
    }
};

class Player
{
public:
    Player (void);

    void OnEvent (const rdge::Event& event);
    void OnUpdate (const rdge::delta_time& dt);

public:
    PlayerStateType state_type = PlayerStateType::Idle;

    input_handler displacement;

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
