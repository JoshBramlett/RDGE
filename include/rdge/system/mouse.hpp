//! \headerfile <rdge/system/mouse.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/19/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>

#include <SDL_mouse.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum MouseButton
//! \brief Represents the different mouse buttons as defined by SDL
//! \note Values represent a bitmask in SDL
enum class MouseButton : uint8
{
    UNKNOWN = 0,
    LEFT    = SDL_BUTTON_LEFT,
    MIDDLE  = SDL_BUTTON_MIDDLE,
    RIGHT   = SDL_BUTTON_RIGHT,
    X1      = SDL_BUTTON_X1,
    X2      = SDL_BUTTON_X2
};

//! \enum SystemCursor
//! \brief Types of all supported system cursors
//! \note Values map directly to SDL_Keycode
//! \warning Not all cursors have correct cross platform mappings
enum class SystemCursor : int32
{
    ARROW     = SDL_SYSTEM_CURSOR_ARROW,     //!< Standard arrow
    IBEAM     = SDL_SYSTEM_CURSOR_IBEAM,     //!< IBeam (text controls)
    WAIT      = SDL_SYSTEM_CURSOR_WAIT,      //!< Wait (hour glass)
    CROSSHAIR = SDL_SYSTEM_CURSOR_CROSSHAIR, //!< Crosshair
    WAITARROW = SDL_SYSTEM_CURSOR_WAITARROW, //!< Small wait (wait if unavailable)
    SIZENWSE  = SDL_SYSTEM_CURSOR_SIZENWSE,  //!< Double arrow pointing NW and SE
    SIZENESW  = SDL_SYSTEM_CURSOR_SIZENESW,  //!< Double arrow pointing NE and SW
    SIZEWE    = SDL_SYSTEM_CURSOR_SIZEWE,    //!< Double arrow pointing W and E
    SIZENS    = SDL_SYSTEM_CURSOR_SIZENS,    //!< Double arrow pointing N and S
    SIZEALL   = SDL_SYSTEM_CURSOR_SIZEALL,   //!< Quad arrow pointing N, E, S, and W
    NO        = SDL_SYSTEM_CURSOR_NO,        //!< Slashed circle or crossbones
    HAND      = SDL_SYSTEM_CURSOR_HAND       //!< Hand
};

//! \class Cursor
//! \brief Wrapper for an SDL_Cursor, which represents a mouse cursor
class Cursor
{
public:
    Cursor (void) = default;

    //! \brief Cursor ctor
    //! \details Create a cursor from the pre-defined list
    //! \param [in] cursor SystemCursor value
    //! \throws rdge::SDLException Cursor cannot be loaded
    explicit Cursor (SystemCursor cursor);

    //!@{
    //! \brief Cursor ctor
    //! \details Load a custom cursor from an existing asset.
    //! \param [in] surface Underlying surface asset
    //! \param [in] hot_x x-coordinate of the cursor hot spot
    //! \param [in] hot_y y-coordinate of the cursor hot spot
    //! \throws rdge::SDLException Cursor cannot be loaded
    explicit Cursor (Surface& surface, int32 hot_x, int32 hot_y);
    explicit Cursor (Surface&& surface, int32 hot_x, int32 hot_y);
    //!@}

    //! \brief Cursor dtor
    ~Cursor (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    Cursor (const Cursor&) = delete;
    Cursor& operator= (const Cursor&) = delete;
    Cursor (Cursor&&) noexcept;
    Cursor& operator= (Cursor&&) noexcept;
    //!@}

    //! \brief User defined conversion to a raw SDL_Cursor pointer
    //! \warning Be careful not to dereference the pointer after the
    //!          parent Cursor object falls out of scope
    //! \returns const pointer to a native SDL_Cursor
    explicit operator const SDL_Cursor* (void) const noexcept
    {
        return m_cursor;
    }

    //! \brief User defined conversion to a raw SDL_Cursor pointer
    //! \returns Pointer to a native SDL_Cursor
    explicit operator SDL_Cursor* (void) const noexcept
    {
        return m_cursor;
    }

private:
    SDL_Cursor* m_cursor = nullptr; //!< Native SDL cursor handle
};


//! \brief Query if the cursor is currently hidden
//! \returns True iff the cursor is hidden, false otherwise
bool IsCursorHidden (void) noexcept;

//! \brief Show the cursor
void ShowCursor (void) noexcept;

//! \brief Hide the cursor
void HideCursor (void) noexcept;

//! \brief Set the current cursor
//! \param [in] cursor Cursor to set
void SetCursor (const Cursor& cursor) noexcept;

//! \brief Set the OS default cursor
void SetDefaultCursor (void) noexcept;

// TODO
// SDL_GetGlobalMouseState
// SDL_GetMouseFocus
// SDL_GetMouseState
// SDL_GetRelativeMouseMode
// SDL_GetRelativeMouseState
// SDL_SetRelativeMouseMode
// SDL_WarpMouseGlobal
// SDL_WarpMouseInWindow - No need, done in Window

//! \brief MouseButton stream output operator
//! \param [in] os Output stream
//! \param [in] button MouseButton enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, MouseButton button);

//! \brief SystemCursor stream output operator
//! \param [in] os Output stream
//! \param [in] cursor SystemCursor enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, SystemCursor cursor);

} // namespace rdge
