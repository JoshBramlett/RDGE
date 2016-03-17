#include <rdge/controls/control.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

Control::Control (const std::string& id, const RDGE::Graphics::Rect& position)
    : m_id(id)
    , m_position(position)
    , m_hasFocus(false)
    , m_disabled(false)
    , m_hasMouseEntered(false)
    , m_hasMouseButtonDown(false)
{ }

void
Control::HandleEvents (const SDL_Event& event)
{
    // TODO: Currently Unsupported
    //
    //       DoubleClick
    //       MouseWheel
    //       GotFocus
    //       LostFocus
    //       KeyDown
    //       KeyUp
    //       KeyPress

    if (m_disabled)
    {
        return;
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        RDGE::Int32 x, y;
        SDL_GetMouseState(&x, &y);

        bool inside = m_position.Contains(x, y);
        if (inside && !m_hasMouseEntered)
        {
            // reset states
            m_hasMouseEntered = true;
            m_hasMouseButtonDown = false;

            TriggerEvent(ControlEventType::MouseEnter, {m_id});
        }
        else if (!inside && m_hasMouseEntered)
        {
            m_hasMouseEntered = false;
            m_hasMouseButtonDown = false;

            TriggerEvent(ControlEventType::MouseLeave, {m_id});
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (m_hasMouseEntered)
        {
            m_hasMouseButtonDown = true;

            TriggerEvent(ControlEventType::MouseDown, {m_id});
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        if (m_hasMouseEntered)
        {
            if (m_hasMouseButtonDown)
            {
                TriggerEvent(ControlEventType::Click, {m_id});
            }

            m_hasMouseButtonDown = false;
        }
    }
}

void
Control::Disable (void)
{
    m_disabled = true;

    // reset state variables
    m_hasMouseEntered    = false;
    m_hasMouseButtonDown = false;
}

bool
Control::IsMouseHover (void)
{
    return m_hasMouseEntered;
}

bool
Control::IsMousePressed (void)
{
    return m_hasMouseButtonDown;
}

void
Control::Enable (void)
{
    m_disabled = false;
}

void
Control::GiveFocus (void)
{
    m_hasFocus = true;

    TriggerEvent(ControlEventType::GotFocus, {m_id});
}

void
Control::RemoveFocus (void)
{
    m_hasFocus = false;

    TriggerEvent(ControlEventType::LostFocus, {m_id});
}

void
Control::RegisterEventHandler (
                               ControlEventType     type,
                               ControlEventCallback handler
                              )
{
    m_subscriptions[type] = handler;
}

void
Control::TriggerEvent (ControlEventType type, const ControlEventArgs& args)
{
    auto iter = m_subscriptions.find(type);
    if (iter != m_subscriptions.end())
    {
        iter->second(args);
    }
}

} // namespace Controls
} // namespace RDGE
