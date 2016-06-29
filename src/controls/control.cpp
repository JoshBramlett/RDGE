#include <rdge/controls/control.hpp>

#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

using namespace RDGE::Graphics;
using namespace RDGE::Math;

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

Control::Control (std::string id, float x, float y)
    : RDGE::Graphics::Group(mat4::translation(vec3(x, y, 0.0f)))
    , m_id(std::move(id))
    , m_disabled(false)
    , m_hasFocus(false)
    , m_isMouseOver(false)
    , m_isLeftMouseButtonDown(false)
    , m_isRightMouseButtonDown(false)
{ }

Control::Control (Control&& rhs) noexcept
    : m_id(std::move(rhs.m_id))
    , m_disabled(rhs.m_disabled)
    , m_hasFocus(rhs.m_hasFocus)
    , m_isMouseOver(rhs.m_isMouseOver)
    , m_isLeftMouseButtonDown(rhs.m_isLeftMouseButtonDown)
    , m_isRightMouseButtonDown(rhs.m_isRightMouseButtonDown)
    , m_subscriptions(std::move(rhs.m_subscriptions))
{ }

Control&
Control::operator= (Control&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_id = std::move(rhs.m_id);
        m_disabled = rhs.m_disabled;
        m_hasFocus = rhs.m_hasFocus;
        m_isMouseOver = rhs.m_isMouseOver;
        m_isLeftMouseButtonDown = rhs.m_isLeftMouseButtonDown;
        m_isRightMouseButtonDown = rhs.m_isRightMouseButtonDown;
        m_subscriptions = std::move(rhs.m_subscriptions);
    }

    return *this;
}

void
Control::OnMouseMotion (const RDGE::MouseMotionEventArgs& args)
{
    if (m_disabled)
    {
        return;
    }

    auto location = args.HomogeneousCursorLocation();

    // TODO: Group should be updated with either a vec4 or a new impl of a
    //       float rect that signifies the bounding box of the group.
    //       Currently, the m_size coordinates represent the right and
    //       bottom bounds, rather than the size
    auto left = m_position.x;
    auto right = m_size.x;
    auto top = m_position.y;
    auto bottom = m_size.y;

    bool inside = left < location.x &&
                  right > location.x &&
                  top < location.y &&
                  bottom > location.y;

    if (inside != m_isMouseOver)
    {
        TriggerEvent(
                     inside ? ControlEventType::MouseEnter : ControlEventType::MouseLeave,
                     { m_id }
                    );

        m_isMouseOver = inside;
        m_isLeftMouseButtonDown = false;
        m_isRightMouseButtonDown = false;
    }
}

void
Control::OnMouseButton (const RDGE::MouseButtonEventArgs& args)
{
    // TODO: Logic needs to be properly implemented for determining a single
    //       click vs a double click.  Currently, a double click will be preceeded
    //       by a single click, so there should be some "wait" period before
    //       firing the event.  Maybe task based?
    if (m_disabled)
    {
        return;
    }

    if (m_isMouseOver == false || args.Button() != RDGE::MouseButton::Left)
    {
        return;
    }

    if (args.Type() == EventType::MouseButtonUp)
    {
        if (args.IsDoubleClick())
        {
            TriggerEvent(ControlEventType::DoubleClick, { m_id });
        }
        else
        {
            TriggerEvent(ControlEventType::Click, { m_id });
        }

        m_isLeftMouseButtonDown = false;
    }
    else if (args.IsButtonPressed())
    {
        TriggerEvent(ControlEventType::MouseDown, { m_id });

        m_isLeftMouseButtonDown = true;
    }
}

void
Control::HandleEvents (const RDGE::Event& event)
{
    // TODO: Currently Unsupported
    //
    //       MouseWheel
    //       GotFocus
    //       LostFocus
    //       KeyDown
    //       KeyUp
    //       KeyPress

    RDGE::Unused(event);
}

void
Control::Disable (void)
{
    m_disabled = true;

    // reset state variables
    //m_hasMouseEntered    = false;
    //m_hasMouseButtonDown = false;

    // TODO: Fire lost focus
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

    //TriggerEvent(ControlEventType::GotFocus, {m_id});
}

void
Control::RemoveFocus (void)
{
    m_hasFocus = false;

    //TriggerEvent(ControlEventType::LostFocus, {m_id});
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
