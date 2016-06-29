//! \headerfile <rdge/controls/button.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2015
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/cursor.hpp>
#include <rdge/controls/control.hpp>
#include <rdge/graphics/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>

#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

//! \enum ButtonState
//! \brief Supported button style states
//! \details Determines how to render the button
enum class ButtonState : RDGE::UInt32
{
    //! \brief Default
    Normal = 1,
    //! \brief Button is pressed
    Pressed,
    //! \brief Button has focus
    Focus,
    //! \brief Button has mouse hovering over it
    Hover,
    //! \brief Button is disabled
    Disabled
};

//! \class Button
//! \brief GUI button
class Button : public Control
{
public:
    //! \brief Button ctor
    //! \param [in] id Unique ID of the control
    //! \param [in] position Location of the control on the screen
    //! \param [in] texture Background texture
    //! \param [in] clip Texture clip for normal style
    //! \param [in] clip_pressed Texture clip for pressed style
    //! \param [in] clip_focus Texture clip for focus style
    //! \param [in] clip_hover Texture clip for hover style
    //! \param [in] clip_disabled Texture clip for disabled style
    explicit Button (
                     std::string        id,
                     const std::string& sprite_sheet,
                     float              x,
                     float              y,
                     float              width,
                     float              height
                    );

    //! \brief Button dtor
    virtual ~Button (void) { }

    //! \brief Button Copy ctor
    //! \details Non-copyable
    Button (const Button&) = delete;

    //! \brief Button Move ctor
    //! \details Default-movable
    Button (Button&&) noexcept;

    //! \brief Button Copy Assignment Operator
    //! \details Non-copyable
    Button& operator=(const Button&) = delete;

    //! \brief Button Move Assignment Operator
    //! \details Default-movable
    Button& operator=(Button&&) noexcept;

    virtual void HandleEvents (const RDGE::Event& event) override;

protected:

    //! \brief Inherited TriggerEvent from Control
    //! \detailis Used to intercept events from the base class to
    //!           the subscribers
    virtual void TriggerEvent (ControlEventType type, const ControlEventArgs& args) override;

private:
    std::shared_ptr<RDGE::Graphics::SpriteSheet> m_spriteSheet;
    std::shared_ptr<RDGE::Graphics::Sprite>      m_sprite;

    RDGE::Cursor m_cursor;
};

} // namespace Controls
} // namespace RDGE
