#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/physics.hpp>
#include <rdge/physics/motion.hpp>

#include "cardinal_direction_animation.hpp"

#include <vector>
#include <memory>

class Player
{
public:
    Player (void);

    void InitPhysics (rdge::physics::CollisionGraph& graph, float inv_ratio);
    void OnEvent (const rdge::Event& event);
    void OnUpdate (const rdge::delta_time& dt);

    rdge::math::vec2 GetWorldCenter (void) const noexcept;

public:
    rdge::Animation* current_animation = nullptr;
    CardinalDirectionAnimation cd_anim_blink;
    CardinalDirectionAnimation cd_anim_walk;
    CardinalDirectionAnimation cd_anim_run;
    CardinalDirectionAnimation cd_anim_sheathe;
    CardinalDirectionAnimation cd_anim_fight;

    std::shared_ptr<rdge::Sprite> sprite;
    rdge::physics::RigidBody* body;

private:
    // input handling
    rdge::KeyboardDirectionalInputHandler m_handler;
    rdge::math::vec2 m_direction;
    rdge::Direction m_facing = rdge::Direction::NONE;

    enum StateFlags
    {
        RUN_BUTTON_PRESSED = 0x0001
    };

    rdge::uint16 m_flags = 0;
};
