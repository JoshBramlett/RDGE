#include <rdge/controls/button.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/color.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

using namespace RDGE::Graphics;

Button::Button (
                std::string        id,
                const std::string& sprite_sheet,
                float              x,
                float              y,
                float              width,
                float              height
               )
    : Control(std::move(id), 0, 0)
{
    m_spriteSheet = std::make_shared<SpriteSheet>(sprite_sheet);
    auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Normal));

    m_sprite = std::make_shared<Sprite>(x, y, width, height, m_spriteSheet, uv);
    AddRenderable(m_sprite);
}

Button::Button (Button&& rhs) noexcept
    : Control(std::move(rhs))
    , m_spriteSheet(std::move(rhs.m_spriteSheet))
    , m_sprite(std::move(rhs.m_sprite))
{ }

Button&
Button::operator= (Button&& rhs) noexcept
{
    if (this != &rhs)
    {
        Control::operator=(std::move(rhs));
        m_spriteSheet = std::move(rhs.m_spriteSheet);
        m_sprite = std::move(rhs.m_sprite);
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
Button::HandleEvents (const RDGE::Event& event)
{
    Control::HandleEvents(event);

    // TODO dirty state so we don't have to update this logic every frame?
    if (m_disabled)
    {
        auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Disabled));
        m_sprite->SetUV(uv);
    }
    else if (m_isLeftMouseButtonDown)
    {
        auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Pressed));
        m_sprite->SetUV(uv);
    }
    else if (m_isMouseOver)
    {
        auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Hover));
        m_sprite->SetUV(uv);
    }
    else if (m_hasFocus)
    {
        auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Focus));
        m_sprite->SetUV(uv);
    }
    else
    {
        auto uv =  m_spriteSheet->LookupUV(static_cast<RDGE::UInt8>(ButtonState::Normal));
        m_sprite->SetUV(uv);
    }
}

} // namespace Controls
} // namespace RDGE
