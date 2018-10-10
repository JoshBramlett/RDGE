#pragma once

#include <chrono/types.hpp>
#include <rdge/type_traits.hpp>

//!@{ Forward declarations
namespace rdge {
class CustomEventArgs;
} // namespace rdge
//!@}

namespace perch {

//! \brief Game client supported custom events
enum class CustomEventType
{
    NONE       = 0,
    DIALOG     = 1,
    SCENE_PUSH = 2,
    SCENE_POP  = 3,
    SCENE_SWAP = 4,

    COUNT      = 5
};

//! \brief Array indexed by \ref CustomEventType
template <typename T>
class CustomEventTypeArray
{
public:
    T& operator[] (CustomEventType t) { return m_arr[rdge::to_underlying(t)]; }
    const T& operator[] (CustomEventType t) const { return m_arr[rdge::to_underlying(t)]; }

private:
    T m_arr[rdge::to_underlying(CustomEventType::COUNT)];
};


//! \brief Game event handling setup
//! \details Disables unused events and sets up the custom event handling.  This
//!          must be callled prior to running the game.
void SetupEvents (void);

//!@{ Custom Events
void EnqueueScenePushEvent (chrono_scene_id scene_id);
void EnqueueScenePopEvent (void);
bool ProcessCustomEvent (const rdge::CustomEventArgs& args);
//!@}

//! \brief CustomEventType stream output operator
std::ostream& operator<< (std::ostream&, CustomEventType);

//!@{ Direction string conversions
bool try_parse (const std::string&, CustomEventType&);
std::string to_string (CustomEventType);
//!@}

} // namespace perch
