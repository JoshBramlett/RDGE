//! \headerfile <rdge/controls/control.hpp>
//! \author Josh Bramlett
//! \version 0.0.7
//! \date 05/25/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/gameobjects/ientity.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL.h>

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
    RDGE::Graphics::Point mouse_position;
    MouseButton           mouse_button;
};

//! \typedef ControlEventCallback
//! \brief Callback subscriber for control events
using ControlEventCallback = std::function<void(void*, const ControlEventArgs&)>;

//! \class Control
//! \brief Base class for GUI controls
//! \details Non-implementable class defining all control behavior
class Control : public RDGE::Graphics::Renderable2D, public RDGE::GameObjects::IEntity
{
public:
    //! \brief IEntity HandleEvents
    //! \details Handle input events to map to control events
    //! \param [in] event SDL_Event struct
    virtual void HandleEvents (const SDL_Event& event);

    //! \brief IEntity Update
    //! \details Implementation logic should be handled in derived class
    virtual void Update (RDGE::UInt32) { }

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
    //! \param [in] position Position of the control in screen coordinates
    explicit Control (const std::string& id, const RDGE::Graphics::Rect& position);

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
