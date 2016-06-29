//! \headerfile <rdge/events/event.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/math/vec2.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \enum EventType
//! \brief Type of event triggered
//! \details Direct mapping to SDL_EventType, provided for abstraction and
//!          ease of discovery.
enum class EventType : RDGE::UInt32
{
    // IMPORTANT - If enum is updated, update ostream operator<< as well

    /* Application Events */

    //! \var Quit
    //! \brief User requested quit
    Quit                    = SDL_QUIT,

    /* Application Events - Android, iOS and WinRT */

    //! \var AppTerminating
    //! \brief OS is terminating the application
    AppTerminating          = SDL_APP_TERMINATING,
    //! \var AppLowMemory
    //! \brief OS is low on memory (attempt to free)
    AppLowMemory            = SDL_APP_LOWMEMORY,
    //! \var AppWillEnterBackground
    //! \brief Application is entering background
    AppWillEnterBackground  = SDL_APP_WILLENTERBACKGROUND,
    //! \var AppDidEnterBackground
    //! \brief Application entered background
    AppDidEnterBackground   = SDL_APP_DIDENTERBACKGROUND,
    //! \var AppWillEnterForeground
    //! \brief Application is entering foreground
    AppWillEnterForeground  = SDL_APP_WILLENTERFOREGROUND,
    //! \var AppDidEnterForeground
    //! \brief Application entered foreground
    AppDidEnterForeground   = SDL_APP_DIDENTERFOREGROUND,

    /* Window Events */

    //! \var Window
    //! \brief Window state change
    Window                  = SDL_WINDOWEVENT,
    //! \var SystemWindowManagement
    //! \brief Video driver dependent system event
    SystemWindowManagement  = SDL_SYSWMEVENT,

    /* Keyboard Events */

    //! \var KeyDown
    //! \brief Key pressed
    KeyDown                 = SDL_KEYDOWN,
    //! \var KeyUp
    //! \brief Key released
    KeyUp                   = SDL_KEYUP,
    //! \var TextEditing
    //! \brief Keyboard text editing (composition)
    TextEditing             = SDL_TEXTEDITING,
    //! \var TextInput
    //! \brief Keyboard text input
    TextInput               = SDL_TEXTINPUT,
    //! \var KeyMapChanged
    //! \brief Mapping changed due to system event (language or keyboard layout change)
    KeyMapChanged           = SDL_KEYMAPCHANGED,

    /* Mouse Events */

    //! \var MouseMotion
    //! \brief Mouse moved
    MouseMotion             = SDL_MOUSEMOTION,
    //! \var MouseButtonDown
    //! \brief Mouse button pressed
    MouseButtonDown         = SDL_MOUSEBUTTONDOWN,
    //! \var MouseButtonUp
    //! \brief Mouse button released
    MouseButtonUp           = SDL_MOUSEBUTTONUP,
    //! \var MouseWheel
    //! \brief Mouse wheel motion
    MouseWheel              = SDL_MOUSEWHEEL,

    /* Joystick Events */

    //! \var JoystickAxisMotion
    //! \brief Joystick axis motion
    JoystickAxisMotion      = SDL_JOYAXISMOTION,
    //! \var JoystickBallMotion
    //! \brief Joystick trackball motion
    JoystickBallMotion      = SDL_JOYBALLMOTION,
    //! \var JoystickHatMotion
    //! \brief Joystick hat position change
    JoystickHatMotion       = SDL_JOYHATMOTION,
    //! \var JoystickButtonDown
    //! \brief Joystick button pressed
    JoystickButtonDown      = SDL_JOYBUTTONDOWN,
    //! \var JoystickButtonUp
    //! \brief Joystick button released
    JoystickButtonUp        = SDL_JOYBUTTONUP,
    //! \var JoystickDeviceAdded
    //! \brief Joystick connected
    JoystickDeviceAdded     = SDL_JOYDEVICEADDED,
    //! \var JoystickDeviceRemoved
    //! \brief Joystick disconnected
    JoystickDeviceRemoved   = SDL_JOYDEVICEREMOVED,

    /* Controller Events */

    //! \var ControllerAxisMotion
    //! \brief Controller axis motion
    ControllerAxisMotion    = SDL_CONTROLLERAXISMOTION,
    //! \var ControllerButtonDown
    //! \brief Controller button pressed
    ControllerButtonDown    = SDL_CONTROLLERBUTTONDOWN,
    //! \var ControllerButtonUp
    //! \brief Controller button released
    ControllerButtonUp      = SDL_CONTROLLERBUTTONUP,
    //! \var ControllerDeviceAdded
    //! \brief Controller connected
    ControllerDeviceAdded   = SDL_CONTROLLERDEVICEADDED,
    //! \var ControllerDeviceRemoved
    //! \brief Controller disconnected
    ControllerDeviceRemoved = SDL_CONTROLLERDEVICEREMOVED,
    //! \var ControllerDeviceMapped
    //! \brief Controller mapping updated
    ControllerDeviceMapped  = SDL_CONTROLLERDEVICEREMAPPED,

    /* Touch Events */

    //! \var FingerDown
    //! \brief Trackpad touched
    FingerDown              = SDL_FINGERDOWN,
    //! \var FingerUp
    //! \brief Trackpad no longer touched
    FingerUp                = SDL_FINGERUP,
    //! \var FingerMotion
    //! \brief Touch drag on trackpad
    FingerMotion            = SDL_FINGERMOTION,

    /* Gesture Events */

    //! \var DollarGesture
    DollarGesture           = SDL_DOLLARGESTURE,
    //! \var DollarRecord
    DollarRecord            = SDL_DOLLARRECORD,
    //! \var MultiGesture
    MultiGesture            = SDL_MULTIGESTURE,

    /* Clipboard Events */

    //! \var ClipboardUpdate
    //! \brief System clipboard changed
    ClipboardUpdate         = SDL_CLIPBOARDUPDATE,

    /* Drag and Drop Events */

    //! \var DropFile
    //! \brief System requests a file open
    DropFile                = SDL_DROPFILE,

    /* Audio Hotplug Events */

    //! \var AudioDeviceAdded
    //! \brief New audio device available
    AudioDeviceAdded        = SDL_AUDIODEVICEADDED,
    //! \var AudioDeviceRemoved
    //! \brief Audio device has been disconnected
    AudioDeviceRemoved      = SDL_AUDIODEVICEREMOVED,

    /* Render Events */

    //! \var RenderTargetsReset
    //! \brief Render targets reset - their contents must be updated
    RenderTargetsReset      = SDL_RENDER_TARGETS_RESET,
    //! \var RenderDeviceReset
    //! \brief Render device reset - all textures must be recreated
    RenderDeviceReset       = SDL_RENDER_DEVICE_RESET
};

//! \enum KeyCode
//! \brief SDL defined virtual key representation
//! \details Direct mapping to SDL_Keycode, provided for abstraction and
//!          ease of discovery.
enum class KeyCode : RDGE::UInt32
{
    // IMPORTANT - If enum is updated, update ostream operator<< as well

    // TODO: The only KeyCode values supported are those on my Mac.  When I move to
    //       full cross platform support I need to add the remainder from here:
    //       https://wiki.libsdl.org/SDLKeycodeLookup
    Unknown    = SDLK_UNKNOWN,
    Backspace  = SDLK_BACKSPACE,
    Tab        = SDLK_TAB,
    Return     = SDLK_RETURN,
    Escape     = SDLK_ESCAPE,
    Space      = SDLK_SPACE,
    Quote      = SDLK_QUOTE,
    Comma      = SDLK_COMMA,
    Minus      = SDLK_MINUS,
    Period     = SDLK_PERIOD,
    Slash      = SDLK_SLASH,
    Zero       = SDLK_0,
    One        = SDLK_1,
    Two        = SDLK_2,
    Three      = SDLK_3,
    Four       = SDLK_4,
    Five       = SDLK_5,
    Six        = SDLK_6,
    Seven      = SDLK_7,
    Eight      = SDLK_8,
    Nine       = SDLK_9,
    Semicolon  = SDLK_SEMICOLON,
    Equals     = SDLK_EQUALS,
    BackQuote  = SDLK_BACKQUOTE,
    A          = SDLK_a,
    B          = SDLK_b,
    C          = SDLK_c,
    D          = SDLK_d,
    E          = SDLK_e,
    F          = SDLK_f,
    G          = SDLK_g,
    H          = SDLK_h,
    I          = SDLK_i,
    J          = SDLK_j,
    K          = SDLK_k,
    L          = SDLK_l,
    M          = SDLK_m,
    N          = SDLK_n,
    O          = SDLK_o,
    P          = SDLK_p,
    Q          = SDLK_q,
    R          = SDLK_r,
    S          = SDLK_s,
    T          = SDLK_t,
    U          = SDLK_u,
    V          = SDLK_v,
    W          = SDLK_w,
    X          = SDLK_x,
    Y          = SDLK_y,
    Z          = SDLK_z,
    CapsLock   = SDLK_CAPSLOCK,
    F1         = SDLK_F1,
    F2         = SDLK_F2,
    F3         = SDLK_F3,
    F4         = SDLK_F4,
    F5         = SDLK_F5,
    F6         = SDLK_F6,
    F7         = SDLK_F7,
    F8         = SDLK_F8,
    F9         = SDLK_F9,
    F10        = SDLK_F10,
    F11        = SDLK_F11,
    F12        = SDLK_F12,
    Right      = SDLK_RIGHT,
    Left       = SDLK_LEFT,
    Down       = SDLK_DOWN,
    Up         = SDLK_UP,
    LeftCtrl   = SDLK_LCTRL,
    LeftShift  = SDLK_LSHIFT,
    LeftAlt    = SDLK_LALT,
    LeftGUI    = SDLK_LGUI,
    RightCtrl  = SDLK_RCTRL,
    RightShift = SDLK_RSHIFT,
    RightAlt   = SDLK_RALT,
    RightGUI   = SDLK_RGUI
};

//! \enum MouseButton
//! \brief Represents the different mouse buttons as defined by SDL
enum class MouseButton : RDGE::UInt8
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

    //! \brief Keyboard button triggering the event
    //! \returns KeyboardButton enum
    constexpr KeyCode Key (void) const
    {
        return static_cast<KeyCode>(keysym.sym);
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
    constexpr RDGE::Graphics::Point CursorLocation (void) const
    {
        return RDGE::Graphics::Point(x, y);
    }

    //! \brief Cursor location in normalized device coordinates
    //! \returns vec2 containing the NDC
    //! \note If the call to get the window (which is used in the calculation)
    //!       fails, the value returned will be {0.0f, 0.0f}
    RDGE::Math::vec2 CursorLocationInNDC (void) const;

    //! \brief Device identifier of the mouse
    //! \returns Device identifier as stored in SDL
    constexpr RDGE::UInt32 DeviceID (void) const
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
    constexpr const RDGE::Math::vec2& HomogeneousCursorLocation (void) const
    {
        return m_homogeneousCursorLocation;
    }

    //! \brief Set the cursor location in homogeneous coordinates
    //! \param [in] cursor 2D vector containing the homogeneous coordinates
    void SetHomogeneousCursorLocation (const RDGE::Math::vec2& cursor)
    {
        m_homogeneousCursorLocation = cursor;
    }

private:
    RDGE::Math::vec2 m_homogeneousCursorLocation;
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
        return static_cast<bool>(state & SDL_BUTTON(static_cast<RDGE::UInt32>(button)));
    }

    //! \brief Cursor location in window coordinates
    //! \returns Point object containing the window coordinates
    constexpr RDGE::Graphics::Point CursorLocation (void) const
    {
        return RDGE::Graphics::Point(x, y);
    }

    //! \brief Cursor location in normalized device coordinates
    //! \returns vec2 containing the NDC
    //! \note If the call to get the window (which is used in the calculation)
    //!       fails, the value returned will be {0.0f, 0.0f}
    RDGE::Math::vec2 CursorLocationInNDC (void) const;

    //! \brief Motion relative to the last event call
    //! \returns Point object containing the relative x and y values
    constexpr RDGE::Graphics::Point RelativeMotion (void) const
    {
        return RDGE::Graphics::Point(xrel, yrel);
    }

    //! \brief Device identifier of the mouse
    //! \returns Device identifier as stored in SDL
    constexpr RDGE::UInt32 DeviceID (void) const
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
    constexpr const RDGE::Math::vec2& HomogeneousCursorLocation (void) const
    {
        return m_homogeneousCursorLocation;
    }

    //! \brief Set the cursor location in homogeneous coordinates
    //! \param [in] cursor 2D vector containing the homogeneous coordinates
    void SetHomogeneousCursorLocation (const RDGE::Math::vec2& cursor)
    {
        m_homogeneousCursorLocation = cursor;
    }

private:
    RDGE::Math::vec2 m_homogeneousCursorLocation;
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
    constexpr RDGE::Int32 HorizontalScroll (void)
    {
        return x;
    }

    //! \brief The amount scrolled vertically
    //! \details Negative towards the user, positive away from the user
    //! \returns Vertical scroll amount
    constexpr RDGE::Int32 VerticalScroll (void)
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
    constexpr RDGE::UInt32 DeviceID (void) const
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
    RDGE::UInt32 Type (void) const
    {
        return type;
    }

    //! \brief The user defined event code
    //! \returns User defined code
    constexpr RDGE::Int32 Code (void)
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
//! \returns Event state prior to change
bool SetEventState (EventType type, bool enable);

//! \brief Request a unique identifier for a custom event
//! \details Registering an event pulls a unique id from the available pool.
//!          This id represents the event type (which for system events is
//!          the EventType enumeration).  This id must be used when pushing
//!          the custom event on the queue.
//! \returns Unique identifier for a custom event
//! \throws RDGE::Exception Custom event pool is exhausted
RDGE::UInt32 RegisterCustomEvent (void);

//! \brief Add a custom event on the queue
//! \param [in] type Registered event type id
//! \param [in] code User defined code
//! \param [in] data1 User defined data pointer
//! \param [in] data2 User defined data pointer
//! \throws RDGE::SDLException Error occured while pushing event on the queue
void QueueCustomEvent (
                       RDGE::UInt32 type,
                       RDGE::Int32  code,
                       void*        data1 = nullptr,
                       void*        data2 = nullptr
                      );

//! \brief EventType stream output operator
//! \param [in] os Output stream
//! \param [in] type EventType enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, EventType type);

//! \brief KeyCode stream output operator
//! \param [in] os Output stream
//! \param [in] key KeyCode enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, KeyCode key);

//! \brief MouseButton stream output operator
//! \param [in] os Output stream
//! \param [in] button MouseButton enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, MouseButton button);

} // namespace RDGE
