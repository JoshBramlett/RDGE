//! \headerfile <rdge/controls/control.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/layers/group.hpp>
#include <rdge/gameobjects/ientity.hpp>

#include <string>
#include <functional>
#include <unordered_map>

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
    std::string           id;
    //RDGE::Graphics::Point mouse_position;
    //MouseButton           mouse_button;
};

//! \typedef ControlEventCallback
//! \brief Callback subscriber for control events
using ControlEventCallback = std::function<void(const ControlEventArgs&)>;

//! \class Control
//! \brief Base class for GUI controls
//! \details Non-implementable class defining all control behavior
class Control : public RDGE::Graphics::Group, public RDGE::GameObjects::IEntity
{
public:
    //! \brief IEntity HandleEvents
    //! \details Handle input events to map to control events
    //! \param [in] event Event arguments
    virtual void HandleEvents (const RDGE::Event& event) override;
    virtual void OnMouseMotion (const RDGE::MouseMotionEventArgs& args);
    virtual void OnMouseButton (const RDGE::MouseButtonEventArgs& args);

    //! \brief IEntity Update
    //! \details Implementation logic should be handled in derived class
    virtual void Update (RDGE::UInt32) override { }

    //! \brief Get the ID of the control
    //! \returns String identifier
    virtual const std::string& ID (void) const final
    {
        return m_id;
    }

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
    //! \param [in] x x-coordinate
    //! \param [in] y y-coordinate
    explicit Control (std::string id, float x, float y);

    //! \brief Control dtor
    virtual ~Control (void) { }

    //! \brief Control Copy ctor
    //! \details Non-copyable
    Control (const Control&) = delete;

    //! \brief Control Move ctor
    //! \details Default-movable
    Control (Control&&) noexcept;

    //! \brief Control Copy Assignment Operator
    //! \details Non-copyable
    Control& operator= (const Control&) = delete;

    //! \brief Control Move Assignment Operator
    //! \details Default-movable
    Control& operator= (Control&&) noexcept;

    //! \brief Control event occurred
    //! \details Inform subscriber of event
    //! \param [in] type Type of event to which occurred
    //! \param [in] args Arguments to pass to the subscriber
    virtual void TriggerEvent (ControlEventType type, const ControlEventArgs& args);

protected:
    std::string m_id;

    bool m_disabled;
    bool m_hasFocus;
    bool m_isMouseOver;
    bool m_isLeftMouseButtonDown;
    bool m_isRightMouseButtonDown;

private:
    std::unordered_map<ControlEventType, ControlEventCallback> m_subscriptions;
};

} // namespace Controls
} // namespace RDGE
