#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>

#include "../../displacement.hpp"

#include <vector>
#include <memory>

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
