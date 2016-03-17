//! \headerfile <rdge/controls/button.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 02/08/2015
//! \bug

#pragma once

#include <rdge/cursor.hpp>
#include <rdge/texture.hpp>
#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/controls/control.hpp>
#include <rdge/graphics/rect.hpp>

#include <memory>
#include <unordered_map>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Controls {

//! \enum ButtonStyle
//! \brief Supported button style states
//! \details Determines how to render the button
enum class ButtonStyle : RDGE::UInt32
{
    //! \brief Normal style
    Normal = 1,
    //! \brief Style when button is pressed
    Pressed,
    //! \brief Style when button has focus
    Focus,
    //! \brief Style when button has mouse hovering over it
    Hover,
    //! \brief Style when disabled
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
                     const std::string&             id,
                     const RDGE::Graphics::Rect&    position,
                     std::shared_ptr<RDGE::Texture> texture,
                     const RDGE::Graphics::Rect& clip          = RDGE::Graphics::Rect::Empty(),
                     const RDGE::Graphics::Rect& clip_pressed  = RDGE::Graphics::Rect::Empty(),
                     const RDGE::Graphics::Rect& clip_focus    = RDGE::Graphics::Rect::Empty(),
                     const RDGE::Graphics::Rect& clip_hover    = RDGE::Graphics::Rect::Empty(),
                     const RDGE::Graphics::Rect& clip_disabled = RDGE::Graphics::Rect::Empty()
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

    //! \brief Entity Render
    //! \details Renders button based on style
    //! \param [in] window Window to render to
    virtual void Render (const RDGE::Window& window);

    //! \brief Entity Tag
    //! \returns Tag to signify entity group
    virtual std::string Tag (void) const
    {
        return "Button";
    }

protected:

    //! \brief Inherited TriggerEvent from Control
    //! \detailis Used to intercept events from the base class to
    //!           the subscribers
    virtual void TriggerEvent (ControlEventType type, const ControlEventArgs& args);

private:
    std::shared_ptr<RDGE::Texture> m_texture;
    std::unordered_map<ButtonStyle, RDGE::Graphics::Rect> m_clips;
    RDGE::Cursor m_cursor;
};

} // namespace Controls
} // namespace RDGE
