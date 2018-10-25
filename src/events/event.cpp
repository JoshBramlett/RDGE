#include <rdge/events/event.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>

#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>


namespace {

using namespace rdge;

/* IMPORTANT - Observed event functionality
 *
 * Using the Mac trackpad
 *   1) Finger touch events are delivered in addition to mouse events
 *   2) MultiGesture event is delivered in addition to mouse wheel
 */

math::vec2
ConvertWindowCoordsToNDC (uint32 windowID, int32 x, int32 y)
{
    // TODO:  This should be removed.  Mouse events should return their
    //        absolute coords, and there should be a helper to convert to NDC
    //
    //        Might have to think about this more though.  The window Id is
    //        provided by the event, which is used to get the width/height to
    //        perform this calculation.  Maybe include the window Id with the
    //        wrapped event args and let the consumer figure it out?
    //
    //        One last thought:  There is no equivalent of SDL_GetWindowFromID
    //        in RDGE, although it definitely does have some utility.  Consider
    //        wrapping this function and returning an RDGE window.
    auto window = SDL_GetWindowFromID(windowID);
    if (RDGE_UNLIKELY(!window))
    {
        return math::vec2();
    }

    int32 width, height;
    SDL_GetWindowSize(window, &width, &height);

    return math::vec2((2.0f * x) / width - 1.0f, 1.0f - (2.0f * y) / height);
}

#ifdef RDGE_DEBUG
    std::vector<EventType> s_supportedEventTypes {
        EventType::Quit,
        EventType::Window,
        EventType::KeyDown,
        EventType::KeyUp,
        EventType::MouseMotion,
        EventType::MouseButtonDown,
        EventType::MouseButtonUp,
        EventType::MouseWheel,
        EventType::ClipboardUpdate
    };
#endif

} // anonymous namespace

namespace rdge {

math::vec2
MouseButtonEventArgs::CursorLocationInNDC (void) const
{
    return ConvertWindowCoordsToNDC(windowID, x, y);
}

math::vec2
MouseMotionEventArgs::CursorLocationInNDC (void) const
{
    return ConvertWindowCoordsToNDC(windowID, x, y);
}

bool
PollEvent (Event* event)
{
    while (SDL_PollEvent(&event->sdl_event))
    {
#ifdef RDGE_DEBUG
        /* For debug builds, check if the event should be expected. */

        if (event->sdl_event.type >= SDL_USEREVENT)
        {
            return true;
        }

        auto result = std::find(s_supportedEventTypes.begin(),
                                s_supportedEventTypes.end(),
                                static_cast<EventType>(event->sdl_event.type));
        if (result == s_supportedEventTypes.end())
        {
            WLOG() << "Unsupported EventType! type="
                   << static_cast<EventType>(event->sdl_event.type);
        }
#endif
        return true;
    }

    return false;
}

bool
IsEventEnabled (EventType type)
{
    if (type == EventType::TextInput || type == EventType::TextEditing)
    {
        return (SDL_IsTextInputActive() == SDL_TRUE);
    }

    return SDL_EventState(static_cast<uint32>(type), SDL_QUERY);
}

void
SetEventState (EventType type, bool enable)
{
    if (type == EventType::TextInput || type == EventType::TextEditing)
    {
        bool active = (SDL_IsTextInputActive() == SDL_TRUE);
        if (active && !enable)
        {
            SDL_StopTextInput();
        }
        else if (!active && enable)
        {
            SDL_StartTextInput();
        }
    }
    else
    {
        SDL_EventState(static_cast<uint32>(type), enable ? SDL_ENABLE : SDL_DISABLE);
    }

    ILOG() << "Setting Event State"
           << " type=" << type
           << " state=" << (enable ? "enabled" : "disabled");
}

void
EnableEvent (EventType type)
{
    SetEventState(type, true);
}

void
DisableEvent (EventType type)
{
    SetEventState(type, false);
}

uint32
RegisterCustomEvent (void)
{
    auto result = SDL_RegisterEvents(1);
    if (result == ((uint32)-1)) // TODO <-- wtf?
    {
        RDGE_THROW("Cannot create custom event.  Pool is exhausted.");
    }

    return result;
}

void
QueueCustomEvent (uint32 type, int32 code, void* data1, void* data2)
{
    // TODO: If/when event filtering is implemented, The result of SDL_PushEvent
    //       will be zero if the event is to be filtered.  We could change the
    //       return type to bool to signify this.
    SDL_Event event;
    SDL_zero(event);

    event.type = type;
    event.user.code = code;
    event.user.data1 = data1;
    event.user.data2 = data2;
    auto result = SDL_PushEvent(&event);
    if (RDGE_UNLIKELY(result < 0))
    {
        SDL_THROW("Failed to push custom event on queue.", "SDL_PushEvent");
    }
}

// TODO: Add substr before sending to stream to get rid of the enum name
std::ostream& operator<< (std::ostream& os, EventType type)
{
    switch (type)
    {
#define CASE(X) case X: os << #X; break;
        CASE(EventType::Quit)

        CASE(EventType::AppTerminating)
        CASE(EventType::AppLowMemory)
        CASE(EventType::AppWillEnterBackground)
        CASE(EventType::AppDidEnterBackground)
        CASE(EventType::AppWillEnterForeground)
        CASE(EventType::AppDidEnterForeground)

        CASE(EventType::Window)
        CASE(EventType::SystemWindowManagement)

        CASE(EventType::KeyDown)
        CASE(EventType::KeyUp)
        CASE(EventType::TextEditing)
        CASE(EventType::TextInput)
        CASE(EventType::KeyMapChanged)

        CASE(EventType::MouseMotion)
        CASE(EventType::MouseButtonUp)
        CASE(EventType::MouseButtonDown)
        CASE(EventType::MouseWheel)

        CASE(EventType::JoystickAxisMotion)
        CASE(EventType::JoystickBallMotion)
        CASE(EventType::JoystickHatMotion)
        CASE(EventType::JoystickButtonDown)
        CASE(EventType::JoystickButtonUp)
        CASE(EventType::JoystickDeviceAdded)
        CASE(EventType::JoystickDeviceRemoved)

        CASE(EventType::ControllerAxisMotion)
        CASE(EventType::ControllerButtonDown)
        CASE(EventType::ControllerButtonUp)
        CASE(EventType::ControllerDeviceAdded)
        CASE(EventType::ControllerDeviceRemoved)
        CASE(EventType::ControllerDeviceMapped)

        CASE(EventType::FingerDown)
        CASE(EventType::FingerUp)
        CASE(EventType::FingerMotion)

        CASE(EventType::DollarGesture)
        CASE(EventType::DollarRecord)
        CASE(EventType::MultiGesture)

        CASE(EventType::ClipboardUpdate)

        CASE(EventType::DropFile)

        CASE(EventType::AudioDeviceAdded)
        CASE(EventType::AudioDeviceRemoved)

        CASE(EventType::RenderTargetsReset)
        CASE(EventType::RenderDeviceReset)
#undef CASE
        default:
        {
            auto value = static_cast<uint32>(type);
            std::ostringstream ss;
            ss << std::hex << std::uppercase << value;

            if (value >= SDL_USEREVENT && value < SDL_LASTEVENT)
            {
                os << "UserEvent[" << ss.str() << "]";
            }
            else
            {
                os << "NOT_FOUND[" << ss.str() << "]";
            }
        }
    }

    return os;
}

} // namespace rdge
