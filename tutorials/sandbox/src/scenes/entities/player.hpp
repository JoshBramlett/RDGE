#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>

#include <rdge/physics/motion.hpp>

#include <vector>
#include <memory>

class CardinalDirectionAnimation
{
public:
    rdge::Animation& operator[] (rdge::Direction dir)
    {
        return animations[rdge::math::lsb(rdge::to_underlying(dir)) - 1];
    }

    std::vector<rdge::Animation> animations;
};

class Player
{
public:
    Player (void);

    void OnEvent (const rdge::Event& event);
    void OnUpdate (const rdge::delta_time& dt);

public:
    rdge::Animation* current_animation = nullptr;
    CardinalDirectionAnimation cd_anim_blink;
    CardinalDirectionAnimation cd_anim_walk;
    CardinalDirectionAnimation cd_anim_run;
    CardinalDirectionAnimation cd_anim_sheathe;
    CardinalDirectionAnimation cd_anim_fight;

    struct stateful_user_input {
        // user input handler
        rdge::KeyboardDirectionalInputHandler dir_handler;
        bool run_button_pressed = false;

        bool sheathe_button_pressed = false; // TODO temp - for testing
        bool fight_button_pressed = false; // TODO temp - for testing

        // displacement calculations
        rdge::displacement disp;

        // frame states
        rdge::Direction facing = rdge::Direction::NONE;
        rdge::math::vec2 position_offset;
        bool is_moving = false;
        bool is_walking = false;
        bool is_running = false;

        void calculate (const rdge::delta_time& dt)
        {
            auto dir_pair = dir_handler.Calculate();
            position_offset = dir_pair.first;
            facing = dir_pair.second;

            is_moving = position_offset.x != 0.f || position_offset.y != 0.f;
            is_walking = is_moving && !run_button_pressed;
            is_running = is_moving && run_button_pressed;

            disp.coefficient = (is_running) ? 20.f : 10.f;
            position_offset = disp.from_velocity(position_offset, dt);
        }
    } user_input;

    std::shared_ptr<rdge::Sprite> sprite;
};
