#include <chrono/events.hpp>
#include <chrono/globals.hpp>
#include <chrono/scenes/scene_manager.hpp>

#include <rdge/events/event.hpp>
#include <rdge/gameobjects/types.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/debug/assert.hpp>

#include <sstream>
#include <cstring> // strrchr

using namespace rdge;

namespace perch {

void SetupEvents (void)
{
    // Disable unprocessed events
    rdge::DisableEvent(EventType::FingerDown);
    rdge::DisableEvent(EventType::FingerUp);
    rdge::DisableEvent(EventType::FingerMotion);
    rdge::DisableEvent(EventType::MultiGesture);
    rdge::DisableEvent(EventType::TextInput);

    static_assert(enum_sanity_check(CustomEventType::SCENE_SWAP, CustomEventType::COUNT),
                  "CustomEventType sanity check failed");
    auto& event_ids = g_game.custom_events;
    event_ids[CustomEventType::DIALOG]     = rdge::RegisterCustomEvent();
    event_ids[CustomEventType::SCENE_PUSH] = rdge::RegisterCustomEvent();
    event_ids[CustomEventType::SCENE_POP]  = rdge::RegisterCustomEvent();
    event_ids[CustomEventType::SCENE_SWAP] = rdge::RegisterCustomEvent();
}

void
EnqueueScenePushEvent (chrono_scene_id scene_id)
{
    RDGE_ASSERT(scene_id > chrono_scene_none);
    RDGE_ASSERT(scene_id < chrono_scene_count);

    auto event_id = g_game.custom_events[CustomEventType::SCENE_PUSH];
    rdge::QueueCustomEvent(event_id, static_cast<int32>(scene_id));
}

void
EnqueueScenePopEvent (void)
{
    auto event_id = g_game.custom_events[CustomEventType::SCENE_POP];
    rdge::QueueCustomEvent(event_id);
}

bool
ProcessCustomEvent (const rdge::CustomEventArgs& args)
{
    auto event_id = args.Type();
    const auto& event_ids = g_game.custom_events;
    if (event_id == event_ids[CustomEventType::SCENE_PUSH])
    {
        auto scene_id = static_cast<chrono_scene_id>(args.Code());
        RDGE_ASSERT(scene_id > chrono_scene_none);
        RDGE_ASSERT(scene_id < chrono_scene_count);

        perch::PushScene(scene_id);
        return true;
    }
    else if (event_id == event_ids[CustomEventType::SCENE_POP])
    {
        perch::PopScene();
        return true;
    }
    else if (event_id == event_ids[CustomEventType::SCENE_SWAP])
    {
        auto scene_id = static_cast<chrono_scene_id>(args.Code());
        RDGE_ASSERT(scene_id > chrono_scene_none);
        RDGE_ASSERT(scene_id < chrono_scene_count);

        perch::SwapScene(scene_id);
        return true;
    }
    else if (event_id == event_ids[CustomEventType::DIALOG])
    {
        return false;
    }

    RDGE_ASSERT(false);
    return true;
}

std::ostream&
operator<< (std::ostream& os, CustomEventType value)
{
    return os << perch::to_string(value);
}

std::string
to_string (CustomEventType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(CustomEventType::NONE)
        CASE(CustomEventType::DIALOG)
        CASE(CustomEventType::SCENE_PUSH)
        CASE(CustomEventType::SCENE_POP)
        CASE(CustomEventType::SCENE_SWAP)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, CustomEventType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "none")       { out = CustomEventType::NONE;       return true; }
    else if (s == "dialog")     { out = CustomEventType::DIALOG;     return true; }
    else if (s == "scene_push") { out = CustomEventType::SCENE_PUSH; return true; }
    else if (s == "scene_pop")  { out = CustomEventType::SCENE_POP;  return true; }
    else if (s == "scene_swap") { out = CustomEventType::SCENE_SWAP; return true; }

    return false;
}

} // namespace perch
