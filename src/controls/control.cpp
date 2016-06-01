#include <rdge/controls/control.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

// TODO:  Thoughts:
//
// It'd be pretty cool to keep all the SDL types in play and be able to use
// type conversions to modify them for OpenGL.  For example, Point could be
// converted to a vec2.  With this I'd need to solidify storing the aspect
// ratio as a global.
//
// Either a conversion function, or would a method called ToNDC() be more clear?
// With a simple type conversion it may be unintuitive to infer it uses the
// aspect ratio when converting.
//
// Also, make sure I initialize the default z-index to 0 everywhere

Control::Control (const std::string& id, float x, float y, float width, float height)
    , Group(mat4::translation(vec3(x, y, 0.0f)))
    , m_id(id)
    , m_disabled(false)
    , m_hasFocus(false)
    , m_isMouseOver(false)
    , m_isLeftMouseButtonDown(false)
    , m_isRightMouseButtonDown(false)
{
    // TODO: I need to do something with width and height.  Groups calculate these
    //       by the added renderables.  I need the screen rect to determine mouse
    //       events, so since those come as screen coordinates I should have a
    //       pixel-perfect rect to compare to?
    //
    //       Maybe the ctor should take a Rect for the position and we convert
    //       to normalized device coords for the Group base class?
}

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

    // TODO: Fire lost focus
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
