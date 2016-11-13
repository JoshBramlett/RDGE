//! \headerfile <rdge/cursor.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 02/15/2015
//! \bug

#pragma once

#include <string>
#include <unordered_map>

#include <SDL_mouse.h>

#include <rdge/types.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \enum CursorType
//! \brief Type of system cursor
//! \details Direct mapping to SDL_SystemCursor, provided for
//!          ease of discovery
//! \note Not all cursors have correct cross platform mappings
enum class SystemCursor : RDGE::Int32
{
    //! \brief System cursor not set
    NotSet    = -1,
    //! \brief Standard arrow
    Arrow     = SDL_SYSTEM_CURSOR_ARROW,
    //! \brief IBeam
    IBeam     = SDL_SYSTEM_CURSOR_IBEAM,
    //! \brief Wait
    Wait      = SDL_SYSTEM_CURSOR_WAIT,
    //! \brief Crosshair
    Crosshair = SDL_SYSTEM_CURSOR_CROSSHAIR,
    //! \brief Small wait (wait if unavailable)
    WaitArrow = SDL_SYSTEM_CURSOR_WAITARROW,
    //! \brief Double arrow pointing NW and SE
    SizeNWSE  = SDL_SYSTEM_CURSOR_SIZENWSE,
    //! \brief Double arrow pointing NE and SW
    SizeNESW  = SDL_SYSTEM_CURSOR_SIZENESW,
    //! \brief Double arrow pointing W and E
    SizeWE    = SDL_SYSTEM_CURSOR_SIZEWE,
    //! \brief Double arrow pointing N and S
    SizeNS    = SDL_SYSTEM_CURSOR_SIZENS,
    //! \brief Quad arrow pointing N, E, S, and W
    SizeAll   = SDL_SYSTEM_CURSOR_SIZEALL,
    //! \brief Slashed circle or crossbones
    No        = SDL_SYSTEM_CURSOR_NO,
    //! \brief Hand
    Hand      = SDL_SYSTEM_CURSOR_HAND
};

//! \class Cursor
//! \brief Wrapper for SDL_Cursor
//! \details Cursor class is somewhat of a hybrid global/local in that
//!          it stores the current global state, but will allow multiple
//!          instances access only to those resources which they create.
//!          Therefore since any object can instantiate and set the global
//!          cursor, the application must be cognizant of that.  Ideally
//!          the less objects which can manipulate the cursor the better.
//!          A common use case would be both the scene and controls within
//!          that scene will all have their own cursor.
class Cursor
{
public:
    //! \brief Cursor ctor
    explicit Cursor (void) { }

    //! \brief Cursor dtor
    ~Cursor (void);

    //! \brief Cursor Copy ctor
    //! \details Non-copyable
    Cursor (const Cursor&) = delete;

    //! \brief Cursor Move ctor
    //! \details Transer ownership
    Cursor (Cursor&& rhs) noexcept;

    //! \brief Cursor Copy Assignment Operator
    //! \details Non-copyable
    Cursor& operator= (const Cursor&) = delete;

    //! \brief Cursor Move Assignment Operator
    //! \details Transer ownership
    Cursor& operator= (Cursor&& rhs) noexcept;

    //! \brief Create a custom cursor
    //! \details Only creates and stores the custom cursor.  Caller
    //!          must still call Set() to display custom cursor
    //! \param [in] id Unique ID of the cursor
    //! \param [in] path Path to the cursor image
    //! \param [in] hot_x X-coordinate click location
    //! \param [in] hot_y Y-coordinate click location
    //! \throws Cursor could not be created
    void CreateCustomCursor (
                             const std::string& id,
                             const std::string& path,
                             RDGE::Int32        hot_x = 0,
                             RDGE::Int32        hot_y = 0
                            );

    //! \brief Set the cursor
    //! \param [in] cursor \ref SystemCursor to set
    void Set (SystemCursor cursor);

    //! \brief Set the cursor
    //! \param [in] id Custom cursor ID
    void Set (const std::string& id);

    //! \brief Revert the cursor to the previous value
    //! \details If the global cursor has changed since the instance
    //!          Set() method was called, this will be ignored
    void Revert (void);

    //! \brief Hide the cursor
    void Hide (void);

    //! \brief Show the cursor
    void Show (void);

private:
    std::unordered_map<SystemCursor, SDL_Cursor*> m_systemCursors;
    std::unordered_map<std::string, SDL_Cursor*> m_customCursors;

    SystemCursor m_previousSystemCursor;
    std::string  m_previousCustomCursor;
    SystemCursor m_currentSystemCursor;
    std::string  m_currentCustomCursor;
};

} // namespace RDGE

