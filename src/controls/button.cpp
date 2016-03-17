#include <rdge/controls/button.hpp>
#include <rdge/internal/exception_macros.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

Button::Button (
                const std::string&             id,
                const RDGE::Graphics::Rect&    position,
                std::shared_ptr<RDGE::Texture> texture,
                const RDGE::Graphics::Rect&    clip,
                const RDGE::Graphics::Rect&    clip_pressed,
                const RDGE::Graphics::Rect&    clip_focus,
                const RDGE::Graphics::Rect&    clip_hover,
                const RDGE::Graphics::Rect&    clip_disabled
               )
    : Control(id, position)
    , m_texture(std::move(texture))
{
    if (UNLIKELY(m_texture == nullptr))
    {
        RDGE_THROW("Button[" + id + "] texture parameter set to NULL");
    }

    m_clips = {
        {ButtonStyle::Normal,   clip},
        {ButtonStyle::Pressed,  clip_pressed},
        {ButtonStyle::Focus,    clip_focus},
        {ButtonStyle::Hover,    clip_hover},
        {ButtonStyle::Disabled, clip_disabled}
    };
}

Button::Button (Button&& rhs) noexcept
    : Control(std::move(rhs))
{
    m_texture = std::move(rhs.m_texture);
    m_clips.swap(rhs.m_clips);
}

Button&
Button::operator= (Button&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_texture = std::move(rhs.m_texture);
        m_clips.swap(rhs.m_clips);
    }

    return *this;
}

void
Button::TriggerEvent (ControlEventType type, const ControlEventArgs& args)
{
    if (type == ControlEventType::MouseEnter)
    {
        m_cursor.Set(SystemCursor::Hand);
    }
    else if (type == ControlEventType::MouseLeave)
    {
        m_cursor.Revert();
    }

    Control::TriggerEvent(type, args);
}

void
Button::Render (const RDGE::Window& window)
{
    auto clip = m_clips[ButtonStyle::Normal];
    if (IsDisabled())
    {
        auto temp = m_clips[ButtonStyle::Disabled];
        if (temp.IsEmpty() == false)
        {
            clip = temp;
        }
    }
    else if (IsMousePressed())
    {
        auto temp = m_clips[ButtonStyle::Pressed];
        if (temp.IsEmpty() == false)
        {
            clip = temp;
        }
    }
    else if (IsMouseHover())
    {
        auto temp = m_clips[ButtonStyle::Hover];
        if (temp.IsEmpty() == false)
        {
            clip = temp;
        }
    }
    else if (HasFocus())
    {
        auto temp = m_clips[ButtonStyle::Focus];
        if (temp.IsEmpty() == false)
        {
            clip = temp;
        }
    }

    window.Draw(*m_texture.get(), m_position, clip);
}

} // namespace Controls
} // namespace RDGE
