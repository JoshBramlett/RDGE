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
    bool is_dirty = true;
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

class Player
{
public:
    Player (void);

    void OnEvent (const rdge::Event& event);
    void OnUpdate (rdge::uint32 ticks);

public:
    PlayerStateType state_type = PlayerStateType::Idle;
    PlayerFacing facing = PlayerFacing::Front;

    // cached for input transition logic - observed SDL behavior is if a key is held
    // down it'll fire continuous events.  However, while a key is held down if another
    // key is pressed any further keydown events are suppressed.  Therefore we need
    // logic to hold the state.
    bool walking = false;
    bool running = false;

    std::vector<std::unique_ptr<player_state>> states;
    player_state* current_state = nullptr;

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
    void OnUpdate (rdge::uint32 ticks) override;
    void OnRender (void) override;

public:
    rdge::OrthographicCamera camera;

    Player player;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::SpriteLayer background;
};
