#include <chrono/events.hpp>
#include <chrono/globals.hpp>
#include <chrono/scenes/scene_manager.hpp>

#include <rdge/events/event.hpp>
#include <rdge/gameobjects/types.hpp>
#include <rdge/debug/assert.hpp>
#include <rdge/type_traits.hpp>

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

    static_assert(enum_sanity_check(ActionType::SCENE_SWAP, ActionType::COUNT),
                  "ActionType sanity check failed");
    auto& event_ids = g_game.custom_events;
    event_ids[ActionType::DIALOG]     = rdge::RegisterCustomEvent();
    event_ids[ActionType::SCENE_PUSH] = rdge::RegisterCustomEvent();
    event_ids[ActionType::SCENE_POP]  = rdge::RegisterCustomEvent();
    event_ids[ActionType::SCENE_SWAP] = rdge::RegisterCustomEvent();
}

void
EnqueueScenePushEvent (chrono_scene_id scene_id)
{
    RDGE_ASSERT(scene_id > chrono_scene_none);
    RDGE_ASSERT(scene_id < chrono_scene_count);

    auto event_id = g_game.custom_events[ActionType::SCENE_PUSH];
    rdge::QueueCustomEvent(event_id, static_cast<int32>(scene_id));
}

void
EnqueueScenePopEvent (void)
{
    auto event_id = g_game.custom_events[ActionType::SCENE_POP];
    rdge::QueueCustomEvent(event_id);
}

bool
ProcessCustomEvent (const rdge::CustomEventArgs& args)
{
    auto event_id = args.Type();
    const auto& event_ids = g_game.custom_events;
    if (event_id == event_ids[ActionType::SCENE_PUSH])
    {
        auto scene_id = static_cast<chrono_scene_id>(args.Code());
        RDGE_ASSERT(scene_id > chrono_scene_none);
        RDGE_ASSERT(scene_id < chrono_scene_count);

        perch::PushScene(scene_id);
        return true;
    }
    else if (event_id == event_ids[ActionType::SCENE_POP])
    {
        perch::PopScene();
        return true;
    }
    else if (event_id == event_ids[ActionType::SCENE_SWAP])
    {
        auto scene_id = static_cast<chrono_scene_id>(args.Code());
        RDGE_ASSERT(scene_id > chrono_scene_none);
        RDGE_ASSERT(scene_id < chrono_scene_count);

        perch::SwapScene(scene_id);
        return true;
    }
    else if (event_id == event_ids[ActionType::DIALOG])
    {
        return false;
    }

    RDGE_ASSERT(false);
    return true;
}

} // namespace perch
