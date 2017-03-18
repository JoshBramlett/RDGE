//! \headerfile <rdge/events/event.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/keyboard.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum EventType
//! \brief Type of event triggered
//! \details Direct mapping to SDL_EventType, provided for abstraction and
//!          ease of discovery.
enum class EventType : uint32
{
    // IMPORTANT - If enum is updated, update ostream operator<< as well

    //!@{
    //! \brief Application Events
    Quit = SDL_QUIT, //!< User requested quit
    //!@}

    //!@{
    //! \brief Application Events - Android, iOS and WinRT
    AppTerminating         = SDL_APP_TERMINATING,         //!< OS is terminating the application
    AppLowMemory           = SDL_APP_LOWMEMORY,           //!< OS is low on memory (attempt to free)
    AppWillEnterBackground = SDL_APP_WILLENTERBACKGROUND, //!< Application is entering background
    AppDidEnterBackground  = SDL_APP_DIDENTERBACKGROUND,  //!< Application entered background
    AppWillEnterForeground = SDL_APP_WILLENTERFOREGROUND, //!< Application is entering foreground
    AppDidEnterForeground  = SDL_APP_DIDENTERFOREGROUND,  //!< Application entered foreground
    //!@}

    //!@{
    //! \brief Window Events
    Window                 = SDL_WINDOWEVENT, //!< Window state change
    SystemWindowManagement = SDL_SYSWMEVENT,  //!< Video driver dependent system event
    //!@}

    //!@{
    //! \brief Keyboard Events
    KeyDown       = SDL_KEYDOWN,       //!< Key pressed
    KeyUp         = SDL_KEYUP,         //!< Key released
    TextEditing   = SDL_TEXTEDITING,   //!< Keyboard text editing (composition)
    TextInput     = SDL_TEXTINPUT,     //!< Keyboard text input
    KeyMapChanged = SDL_KEYMAPCHANGED, //!< System event (language or keyboard layout change)
    //!@}

    //!@{
    //! \brief Mouse Events
    MouseMotion     = SDL_MOUSEMOTION,     //!< Mouse moved
    MouseButtonDown = SDL_MOUSEBUTTONDOWN, //!< Mouse button pressed
    MouseButtonUp   = SDL_MOUSEBUTTONUP,   //!< Mouse button released
    MouseWheel      = SDL_MOUSEWHEEL,      //!< Mouse wheel motion
    //!@}

    //!@{
    //! \brief Joystick Events
    JoystickAxisMotion    = SDL_JOYAXISMOTION,    //!< Joystick axis motion
    JoystickBallMotion    = SDL_JOYBALLMOTION,    //!< Joystick trackball motion
    JoystickHatMotion     = SDL_JOYHATMOTION,     //!< Joystick hat position change
    JoystickButtonDown    = SDL_JOYBUTTONDOWN,    //!< Joystick button pressed
    JoystickButtonUp      = SDL_JOYBUTTONUP,      //!< Joystick button released
    JoystickDeviceAdded   = SDL_JOYDEVICEADDED,   //!< Joystick connected
    JoystickDeviceRemoved = SDL_JOYDEVICEREMOVED, //!< Joystick disconnected
    //!@}

    //!@{
    //! \brief Controller Events
    ControllerAxisMotion    = SDL_CONTROLLERAXISMOTION,     //!< Controller axis motion
    ControllerButtonDown    = SDL_CONTROLLERBUTTONDOWN,     //!< Controller button pressed
    ControllerButtonUp      = SDL_CONTROLLERBUTTONUP,       //!< Controller button released
    ControllerDeviceAdded   = SDL_CONTROLLERDEVICEADDED,    //!< Controller connected
    ControllerDeviceRemoved = SDL_CONTROLLERDEVICEREMOVED,  //!< Controller disconnected
    ControllerDeviceMapped  = SDL_CONTROLLERDEVICEREMAPPED, //!< Controller mapping updated
    //!@}

    //!@{
    //! \brief Touch Events
    FingerDown   = SDL_FINGERDOWN,   //!< Trackpad touched
    FingerUp     = SDL_FINGERUP,     //!< Trackpad no longer touched
    FingerMotion = SDL_FINGERMOTION, //!< Touch drag on trackpad
    //!@}

    //!@{
    //! \brief Gesture Events
    DollarGesture = SDL_DOLLARGESTURE,
    DollarRecord  = SDL_DOLLARRECORD,
    MultiGesture  = SDL_MULTIGESTURE,
    //!@}

    //!@{
    //! \brief Clipboard Events
    ClipboardUpdate = SDL_CLIPBOARDUPDATE, //!< System clipboard changed
    //!@}

    //!@{
    //! \brief Drag and Drop Events
    DropFile = SDL_DROPFILE, //!< System requests a file open
    //!@}

    //!@{
    //! \brief Audio Hotplug Events
    AudioDeviceAdded   = SDL_AUDIODEVICEADDED,   //!< New audio device available
    AudioDeviceRemoved = SDL_AUDIODEVICEREMOVED, //!< Audio device has been disconnected
    //!@}

    //!@{
    //! \brief Render Events
    RenderTargetsReset = SDL_RENDER_TARGETS_RESET, //!< Render target contents must be updated
    RenderDeviceReset  = SDL_RENDER_DEVICE_RESET   //!< Device reset, all textures must be recreated
    //!@}
};

//! \enum MouseButton
//! \brief Represents the different mouse buttons as defined by SDL
enum class MouseButton : uint8
{
    // IMPORTANT - If enum is updated, update ostream operator<< as well

    Unknown = 0,
    Left    = SDL_BUTTON_LEFT,
    Middle  = SDL_BUTTON_MIDDLE,
    Right   = SDL_BUTTON_RIGHT,
    X1      = SDL_BUTTON_X1,
    X2      = SDL_BUTTON_X2
};

//! \class QuitEventArgs
//! \brief Arguments for a quit event
//! \details A quit event is a signal the application should self-terminate.
//!          Numerous sources can trigger the event, such as the last window
//!          closing, SIGTERM and other signal messages, etc.
class QuitEventArgs final : SDL_QuitEvent
{
public:
    //! \brief QuitEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr QuitEventArgs (const SDL_Event& event)
        : SDL_QuitEvent(event.quit)
    { }

    //! \brief Event type triggering the event
    //! \details Valid values: [Quit].
    //! \returns EventType that triggered the event
    EventType Type (void) const
    {
        return static_cast<EventType>(type);
    }
};

//! \class KeyboardEventArgs
//! \brief Arguments for a keyboard event
// TODO Add KeyMod
class KeyboardEventArgs final : SDL_KeyboardEvent
{
public:
    //! \brief KeyboardEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr KeyboardEventArgs (const SDL_Event& event)
        : SDL_KeyboardEvent(event.key)
    { }

    //! \brief Event type triggering the event
    //! \details Valid values: [KeyDown, KeyUp].
    //! \returns EventType that triggered the event
    EventType Type (void) const
    {
        return static_cast<EventType>(type);
    }

    //! \brief Virtual mapping of the key that triggered the event
    //! \returns KeyCode enum
    constexpr KeyCode Key (void) const
    {
        return static_cast<KeyCode>(keysym.sym);
    }

    //! \brief Physical key that triggered the event
    //! \returns ScanCode enum
    constexpr ScanCode PhysicalKey (void) const
    {
        return static_cast<ScanCode>(keysym.scancode);
    }

    //! \brief Check if event was fired from a repeating action
    //! \returns True if repeating, false otherwise
    constexpr bool IsRepeating (void) const
    {
        return (repeat != 0);
    }

    //! \brief Check if keyboard key is in a pressed state
    //! \returns True if pressed, false otherwise
    constexpr bool IsKeyPressed (void) const
    {
        return (state == SDL_PRESSED);
    }
};

//! \class MouseButtonEventArgs
//! \brief Arguments for a mouse button event
class MouseButtonEventArgs final : SDL_MouseButtonEvent
{
public:
    //! \brief MouseButtonEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr MouseButtonEventArgs (const SDL_Event& event)
        : SDL_MouseButtonEvent(event.button)
    { }

    //! \brief Event type triggering the event
    //! \details Valid values: [MouseButtonUp, MouseButtonDown].
    //! \returns EventType that triggered the event
    EventType Type (void) const
    {
        return static_cast<EventType>(type);
    }

    //! \brief Mouse button triggering the event
    //! \returns MouseButton enum
    constexpr MouseButton Button (void) const
    {
        return static_cast<MouseButton>(button);
    }

    //! \brief Check if event is a result of a double click
    //! \details SDL will increment the click count if the time threshold
    //!          has not been met to restart.  If the click count reaches
    //!          two or higher it is treated as a double click.
    //! \returns True if double click, false if single click
    constexpr bool IsDoubleClick (void) const
    {
        return (clicks >= 2);
    }

    //! \brief Check if mouse button is in a pressed state
    //! \returns True if pressed, false otherwise
    constexpr bool IsButtonPressed (void) const
    {
        return (state == SDL_PRESSED);
    }

    //! \brief Cursor location in window coordinates
    //! \returns Point containing the window coordinates
    constexpr rdge::point CursorLocation (void) const
    {
        return rdge::point(x, y);
    }

    //! \brief Cursor location in normalized device coordinates
    //! \returns vec2 containing the NDC
    //! \note If the call to get the window (which is used in the calculation)
    //!       fails, the value returned will be {0.0f, 0.0f}
    rdge::math::vec2 CursorLocationInNDC (void) const;

    //! \brief Device identifier of the mouse
    //! \returns Device identifier as stored in SDL
    constexpr rdge::uint32 DeviceID (void) const
    {
        return which;
    }

    //! \brief Check if the device is a trackpad
    //! \returns True if trackpad, false otherwise
    constexpr bool IsTouchDevice (void) const
    {
        return (which == SDL_TOUCH_MOUSEID);
    }

    //! \brief Cursor location in homogeneous coordinates
    //! \details Value must be explicitly set before querying.
    //! \returns vec2 object containing the homogeneous coordinates
    constexpr const rdge::math::vec2& HomogeneousCursorLocation (void) const
    {
        return m_homogeneousCursorLocation;
    }

    //! \brief Set the cursor location in homogeneous coordinates
    //! \param [in] cursor 2D vector containing the homogeneous coordinates
    void SetHomogeneousCursorLocation (const rdge::math::vec2& cursor)
    {
        m_homogeneousCursorLocation = cursor;
    }

private:
    rdge::math::vec2 m_homogeneousCursorLocation;
};

//! \class MouseMotionEventArgs
//! \brief Arguments for a mouse motion event
class MouseMotionEventArgs final : SDL_MouseMotionEvent
{
public:
    //! \brief MouseMotionEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr MouseMotionEventArgs (const SDL_Event& event)
        : SDL_MouseMotionEvent(event.motion)
    { }

    //! \brief Event type triggering the event
    //! \details Valid values: [MouseMotion].
    //! \returns EventType that triggered the event
    EventType Type (void) const
    {
        return static_cast<EventType>(type);
    }

    //! \brief Check if mouse button is pressed during motion event
    //! \returns True if pressed, false otherwise
    constexpr bool IsButtonPressed (MouseButton button) const
    {
        return static_cast<bool>(state & SDL_BUTTON(static_cast<rdge::uint32>(button)));
    }

    //! \brief Cursor location in window coordinates
    //! \returns Point object containing the window coordinates
    constexpr rdge::point CursorLocation (void) const
    {
        return rdge::point(x, y);
    }

    //! \brief Cursor location in normalized device coordinates
    //! \returns vec2 containing the NDC
    //! \note If the call to get the window (which is used in the calculation)
    //!       fails, the value returned will be {0.0f, 0.0f}
    rdge::math::vec2 CursorLocationInNDC (void) const;

    //! \brief Motion relative to the last event call
    //! \returns Point object containing the relative x and y values
    constexpr rdge::point RelativeMotion (void) const
    {
        return rdge::point(xrel, yrel);
    }

    //! \brief Device identifier of the mouse
    //! \returns Device identifier as stored in SDL
    constexpr rdge::uint32 DeviceID (void) const
    {
        return which;
    }

    //! \brief Check if the device is a trackpad
    //! \returns True if trackpad, false otherwise
    constexpr bool IsTouchDevice (void) const
    {
        return (which == SDL_TOUCH_MOUSEID);
    }

    //! \brief Cursor location in homogeneous coordinates
    //! \details Value must be explicitly set before querying.
    //! \returns vec2 object containing the homogeneous coordinates
    constexpr const rdge::math::vec2& HomogeneousCursorLocation (void) const
    {
        return m_homogeneousCursorLocation;
    }

    //! \brief Set the cursor location in homogeneous coordinates
    //! \param [in] cursor 2D vector containing the homogeneous coordinates
    void SetHomogeneousCursorLocation (const rdge::math::vec2& cursor)
    {
        m_homogeneousCursorLocation = cursor;
    }

private:
    rdge::math::vec2 m_homogeneousCursorLocation;
};

//! \class MouseWheelEventArgs
//! \brief Arguments for a mouse wheel event
class MouseWheelEventArgs final : SDL_MouseWheelEvent
{
public:
    //! \brief MouseWheelEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr MouseWheelEventArgs (const SDL_Event& event)
        : SDL_MouseWheelEvent(event.wheel)
    { }

    //! \brief Event type triggering the event
    //! \details Valid values: [MouseWheel].
    //! \returns EventType that triggered the event
    EventType Type (void) const
    {
        return static_cast<EventType>(type);
    }

    //! \brief The amount scrolled horizontally
    //! \details Negative to the left, positive to the right
    //! \returns Horizontal scroll amount
    constexpr rdge::int32 HorizontalScroll (void)
    {
        return x;
    }

    //! \brief The amount scrolled vertically
    //! \details Negative towards the user, positive away from the user
    //! \returns Vertical scroll amount
    constexpr rdge::int32 VerticalScroll (void)
    {
        return y;
    }

    //! \brief Check if mouse button is pressed during motion event
    //! \returns True if pressed, false otherwise
    constexpr bool IsWheelFlipped (void) const
    {
        // TODO: Make sure the mouse wheel event works correctly with an inverted wheel.
        //       From the SDL documentation:
        //
        //       https://wiki.libsdl.org/SDL_MouseWheelEvent
        //
        //       SDL does not abstract the mouse wheel scroll directions to be consistent
        //       across all platforms (SDL_MOUSEWHEEL_NORMAL). If direction is
        //       SDL_MOUSEWHEEL_FLIPPED the values in x and y will be opposite. Multiply
        //       by -1 to change them back.
        return (direction == SDL_MOUSEWHEEL_FLIPPED);
    }

    //! \brief Device identifier of the mouse
    //! \returns Device identifier as stored in SDL
    constexpr rdge::uint32 DeviceID (void) const
    {
        return which;
    }

    //! \brief Check if the device is a trackpad
    //! \returns True if trackpad, false otherwise
    constexpr bool IsTouchDevice (void) const
    {
        return (which == SDL_TOUCH_MOUSEID);
    }
};

//! \class CustomEventArgs
//! \brief Arguments for a custom event
class CustomEventArgs final : SDL_UserEvent
{
public:
    //! \brief CustomEventArgs ctor
    //! \details Initialize arguments
    //! \param [in] event Native SDL_Event union
    constexpr CustomEventArgs (const SDL_Event& event)
        : SDL_UserEvent(event.user)
    { }

    //! \brief Event type triggering the event
    //! \details The type is provided by the user when queueing.
    //! \returns Event type that triggered the event
    rdge::uint32 Type (void) const
    {
        return type;
    }

    //! \brief The user defined event code
    //! \returns User defined code
    constexpr rdge::int32 Code (void)
    {
        return code;
    }

    //! \brief User defined data pointer
    //! \returns User defined data pointer
    constexpr void* Data1 (void)
    {
        return data1;
    }

    //! \brief User defined data pointer
    //! \returns User defined data pointer
    constexpr void* Data2 (void)
    {
        return data2;
    }
};

//! \class Event
//! \brief Generic event object passed through the event phase
//! \details Because SDL_Event is a union this cannot be a base class for
//!          events which inherits from SDL_Event.  Instead, this class carries
//!          a member of the SDL_Event union which stores the native event data
//!          and has the ability to query the event type and return the
//!          appropriate event object.
class Event
{
public:
    //! \var sdl_event Native SDL_Event union
    SDL_Event sdl_event;

public:
    //! \brief Event ctor
    Event (void)
    { }

    //! \brief Get the type of the event
    //! \returns EventType enumeration
    EventType Type (void) const
    {
        return static_cast<EventType>(sdl_event.type);
    }

    //! \brief Check if the event is a quit event
    //! \returns True if quit event, false otherwise
    bool IsQuitEvent (void) const
    {
        return static_cast<EventType>(sdl_event.type) == EventType::Quit;
    }

    //! \brief Check if the event is a keyboard event
    //! \returns True if keyboard event, false otherwise
    bool IsKeyboardEvent (void) const
    {
        return static_cast<EventType>(sdl_event.type) == EventType::KeyDown ||
               static_cast<EventType>(sdl_event.type) == EventType::KeyUp;
    }

    //! \brief Check if the event is a mouse motion event
    //! \returns True if mouse motion event, false otherwise
    bool IsMouseMotionEvent (void) const
    {
        return static_cast<EventType>(sdl_event.type) == EventType::MouseMotion;
    }

    //! \brief Check if the event is a mouse button event
    //! \returns True if mouse button event, false otherwise
    bool IsMouseButtonEvent (void) const
    {
        return static_cast<EventType>(sdl_event.type) == EventType::MouseButtonUp ||
               static_cast<EventType>(sdl_event.type) == EventType::MouseButtonDown;
    }

    //! \brief Check if the event is a mouse wheel event
    //! \returns True if mouse wheel event, false otherwise
    bool IsMouseWheelEvent (void) const
    {
        return static_cast<EventType>(sdl_event.type) == EventType::MouseWheel;
    }

    //! \brief Check if the event is a custom event
    //! \returns True if custom event, false otherwise
    bool IsCustomEvent (void) const
    {
        return sdl_event.type >= SDL_USEREVENT;
    }

    //! \brief Get arguments for a quit event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a quit event
    QuitEventArgs GetQuitEventArgs (void) const
    {
        return QuitEventArgs(sdl_event);
    }

    //! \brief Get arguments for a keyboard event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a keyboard event
    KeyboardEventArgs GetKeyboardEventArgs (void) const
    {
        return KeyboardEventArgs(sdl_event);
    }

    //! \brief Get arguments for a mouse motion event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a mouse motion event
    MouseMotionEventArgs GetMouseMotionEventArgs (void) const
    {
        return MouseMotionEventArgs(sdl_event);
    }

    //! \brief Get arguments for a mouse button event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a mouse button event
    MouseButtonEventArgs GetMouseButtonEventArgs (void) const
    {
        return MouseButtonEventArgs(sdl_event);
    }

    //! \brief Get arguments for a mouse wheel event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a mouse wheel event
    MouseWheelEventArgs GetMouseWheelEventArgs (void) const
    {
        return MouseWheelEventArgs(sdl_event);
    }

    //! \brief Get arguments for a custom event
    //! \details Behavior is undefined if called if event type is different.
    //! \returns Arguments for a custom event
    CustomEventArgs GetCustomEventArgs (void) const
    {
        return CustomEventArgs(sdl_event);
    }
};

//! \brief Poll event queue
//! \param [out] event Event object that will be populated upon success
//! \returns True if successful, false if no event is available
bool PollEvent (Event* event);

//! \brief Check if an event type is enabled
//! \param [in] type EventType to query
//! \returns True if enabled, false if not
bool IsEventEnabled (EventType type);

//! \brief Set whether event type will be included in the event queue
//! \param [in] type EventType to set
//! \param [in] enable True to enable, false to disable
void SetEventState (EventType type, bool enable);

//! \brief Enable event type
//! \param [in] type EventType to enable
void EnableEvent (EventType type);

//! \brief Disable event type
//! \param [in] type EventType to disable
void DisableEvent (EventType type);

//! \brief Request a unique identifier for a custom event
//! \details Registering an event pulls a unique id from the available pool.
//!          This id represents the event type (which for system events is
//!          the EventType enumeration).  This id must be used when pushing
//!          the custom event on the queue.
//! \returns Unique identifier for a custom event
//! \throws RDGE::Exception Custom event pool is exhausted
uint32 RegisterCustomEvent (void);

//! \brief Add a custom event on the queue
//! \param [in] type Registered event type id
//! \param [in] code User defined code
//! \param [in] data1 User defined data pointer
//! \param [in] data2 User defined data pointer
//! \throws RDGE::SDLException Error occured while pushing event on the queue
void QueueCustomEvent (uint32 type,
                       int32  code,
                       void*  data1 = nullptr,
                       void*  data2 = nullptr);

//! \brief EventType stream output operator
//! \param [in] os Output stream
//! \param [in] type EventType enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, rdge::EventType type);

//! \brief MouseButton stream output operator
//! \param [in] os Output stream
//! \param [in] button MouseButton enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, rdge::MouseButton button);

} // namespace rdge
