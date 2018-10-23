#pragma once

#include <rdge/math/vec2.hpp>

//!@{ Forward declarations
namespace rdge {
class Event;
struct delta_time;
} // namespace rdge
struct fixture_user_data;
//!@}

class IActor
{
public:
    virtual ~IActor (void) = default;

    virtual void OnEvent (const rdge::Event&) = 0;
    virtual void OnUpdate (const rdge::delta_time&) = 0;
    virtual void OnActionTriggered (const fixture_user_data&) = 0;

    virtual rdge::uint32 GetActorId (void) const noexcept = 0;
    virtual rdge::math::vec2 GetWorldCenter (void) const noexcept = 0;

    static IActor* Extract (const fixture_user_data*);
};

namespace perch {

//!\brief Supported derived IActor types
enum class ActorType
{
    NONE = 0,
    PLAYER = 1,

    // tile objects
    STATIC    = 100,
    SIGN      = 101,
    CONTAINER = 102,

    // npcs
    DEBUTANTE = 1000,
};

//! \brief ActorType stream output operator
std::ostream& operator<< (std::ostream&, ActorType);

//!@{ Direction string conversions
bool try_parse (const std::string&, ActorType&);
std::string to_string (ActorType);
//!@}

} // namespace perch

#if 0

{
  "actor_id": Number,
  "name": String,
  "actor_type": Enum,
  "pos": [ Number, Number ]
  "sprite": {
    "sheet_id": Number,
  },
  "physics": {
    "body": {
      "type": Enum,
    },

  },
}

class Sign : public IActor
{
public:
    explicit Container (const nlohmann::json&);
    ~Container (void) noexcept = default;

    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

public:

    rdge::sprite_data* sprite = nullptr;

    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* collision = nullptr;
    rdge::physics::Fixture* sensor = nullptr;
    fixture_user_data sensor_user_data;

    enum actor_sign_sensor
    {
        actor_sign_sensor_front = 0,
        actor_sign_sensor_back  = 1,

        actor_sign_sensor_count = 2,
    };

    struct sensor_data
    {
        rdge::physics::Fixture* fixture = nullptr;
        fixture_user_data user_data;
    };

    sensor_data[actor_sign_sensor_count] sensors;
};


class Container : public IActor
{
public:
    explicit Container (const nlohmann::json&);
    ~Container (void) noexcept = default;

    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;

    rdge::uint32 GetActorId (void) const noexcept override;
    rdge::math::vec2 GetWorldCenter (void) const noexcept override;

    rdge::sprite_data* sprite = nullptr;

    rdge::physics::RigidBody* body = nullptr;
    rdge::physics::Fixture* collision = nullptr;
    rdge::physics::Fixture* sensor = nullptr;
    fixture_user_data sensor_user_data;

private:
    enum class ContainerState
    {
        CLOSED  = 0,
        OPEN    = 1,
        OPENING = 2
    } m_state;

    rdge::Animation opening_animation;

    enum StateFlags
    {
        RUN_BUTTON_PRESSED    = 0x0001,
        ATTACK_BUTTON_PRESSED = 0x0002,
        INPUT_LOCKED          = 0x0004,
        ATTACKING             = 0x0010
    };

    rdge::uint16 m_flags = 0;
};
#endif
