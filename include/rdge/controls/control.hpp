//! \headerfile <rdge/controls/control.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 02/08/2015
//! \bug

#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/graphics/rect.hpp>
#include <rdge/gameobjects/ientity.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

//! \enum ControlEventType
//! \brief Supported event types
//! \details Provided with callback registration
enum class ControlEventType : RDGE::UInt8
{
    //! \brief Mouse clicked
    Click = 1,
    //! \brief Mouse double clicked
    DoubleClick,
    //! \brief Mouse entered control bounds
    MouseEnter,
    //! \brief Mouse left control bounds
    MouseLeave,
    //! \brief Mouse button down while control has focus
    MouseDown,
    //! \brief Mouse wheel event while control has focus
    MouseWheel,
    //! \brief Control focus gained
    GotFocus,
    //! \brief Control focus lost
    LostFocus,
    //! \brief Key down while control has focus
    KeyDown,
    //! \brief Key up while control has focus
    KeyUp,
    //! \brief Key pressed while control has focus
    KeyPress
};

//! \enum ControlEventArgs
//! \brief Arguments sent with \ref ControlEventCallback
struct ControlEventArgs
{
    std::string id;
};

//TODO: Add object sender to ControlEventCallback
class Control;

//! \typedef ControlEventCallback
//! \brief Callback subscriber for control events
using ControlEventCallback = std::function<void(const ControlEventArgs&)>;

//! \class Control
//! \brief Base class for GUI controls
//! \details Non-implementable class defining all control behavior
class Control : public RDGE::GameObjects::IEntity
{
public:
    //! \brief Control dtor
    virtual ~Control (void) { }

    //! \brief Control Copy ctor
    //! \details Non-copyable
    Control (const Control&) = delete;

    //! \brief Control Move ctor
    //! \details Default-movable
    Control (Control&&) noexcept = default;

    //! \brief Control Copy Assignment Operator
    //! \details Non-copyable
    Control& operator=(const Control&) = delete;

    //! \brief Control Move Assignment Operator
    //! \details Default-movable
    Control& operator=(Control&&) noexcept = default;

    //! \brief IEntity HandleEvents
    //! \details Handle input events to map to control events
    //! \param [in] event SDL_Event struct
    virtual void HandleEvents (const SDL_Event& event);

    //! \brief IEntity Update
    //! \details Implementation logic should be handled in derived class
    virtual void Update (RDGE::UInt32) { }

    //! \brief IEntity Render
    //! \details Implementation logic should be handled in derived class
    virtual void Render (const RDGE::Window&) { }

    //! \brief Disable the control
    virtual void Disable (void);

    //! \brief Enable the control
    virtual void Enable (void);

    //! \brief Get the control disabled state
    //! \returns True if control is disabled, false otherwise
    virtual bool IsDisabled (void) const final
    {
        return m_disabled;
    }

    //! \brief Give the control focus
    //! \details Programmatic access to set the control focus
    virtual void GiveFocus (void);

    //! \brief Remove the control focus
    //! \details Programmatic access to remove the control focus
    virtual void RemoveFocus (void);

    //! \brief Get the control focus state
    //! \returns True if control has focus, false otherwise
    virtual bool HasFocus (void) const final
    {
        return m_hasFocus;
    }

    //! \brief Register a callback function to receive control events
    //! \details Single entry point registration, but can be called multiple
    //!          times to register for different event types.  Calling
    //!          multiple times with the same \ref EventType will
    //!          overwrite the existing callback
    //! \param [in] type Type of event to subscribe to
    //! \param [in] handler Callback called when event is triggered
    virtual void RegisterEventHandler (
                                       ControlEventType     type,
                                       ControlEventCallback handler
                                      ) final;

protected:
    //! \brief Control ctor
    //! \param [in] id Unique ID of the control
    //! \param [in] position Location of the control on the screen
    explicit Control (const std::string& id, const RDGE::Graphics::Rect& position);

    //! \brief Control event occurred
    //! \details Inform subscriber of event
    //! \param [in] type Type of event to which occurred
    //! \param [in] args Arguments to pass to the subscriber
    virtual void TriggerEvent (ControlEventType type, const ControlEventArgs& args);

    //! \brief Is the mouse within the control bounds
    //! \details Utility method for derived classes that allow the base
    //!          to handle game loop events
    virtual bool IsMouseHover (void) final;

    //! \brief Is the mouse within the control bounds & pressed
    //! \details Utility method for derived classes that allow the base
    //!          to handle game loop events
    virtual bool IsMousePressed (void) final;

    std::string          m_id;
    RDGE::Graphics::Rect m_position;

    bool m_hasFocus;
    bool m_disabled;

private:
    std::unordered_map<ControlEventType, ControlEventCallback> m_subscriptions;

    // Internal state information.
    bool m_hasMouseEntered;
    bool m_hasMouseButtonDown;
};

} // namespace Controls
} // namespace RDGE
