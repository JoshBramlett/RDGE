//! \headerfile <rdge/system/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/05/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL_rect.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

// TODO Adding operator overloads on an as needed basis

//! \struct screen_point
//! \brief Structure defining a screen coordinate
//! \see http://wiki.libsdl.org/SDL_Point
struct screen_point : public SDL_Point
{
    //! \brief screen_point ctor
    //! \details Initialize point to [0,0].
    constexpr screen_point (void)
        : SDL_Point{0, 0}
    { }

    //! \brief screen_point ctor
    //! \details Initialize from SDL_Point.
    //! \param [in] sdl_point SDL_Point struct
    explicit constexpr screen_point (const SDL_Point& sdl_point)
        : SDL_Point{sdl_point}
    { }

    //! \brief screen_point ctor
    //! \details Initialize color from x and y values.
    //! \param [in] px x-coordinate
    //! \param [in] py y-coordinate
    constexpr screen_point (int32 px, int32 py)
        : SDL_Point{px, py}
    { }
};

//! \struct screen_rect
//! \brief Structure defining a rectangle in canonical OS screen/display format
//! \details The x/y coordinates which make up the origin are situated in the top
//!          left corner.  Width expands left to right. Height expands top to bottom.
//! \warning Not for use with the graphics system which uses cartesian coordinates.
//! \see http://wiki.libsdl.org/SDL_Rect
struct screen_rect : public SDL_Rect
{
    //! \brief screen_rect ctor
    //! \details Zero initialization
    constexpr screen_rect (void)
        : SDL_Rect{0, 0, 0, 0}
    { }

    //! \brief screen_rect ctor
    //! \details Initialize from SDL_Rect.
    //! \param [in] sdl_rect SDL_Rect struct
    explicit constexpr screen_rect (const SDL_Rect& sdl_rect)
        : SDL_Rect{sdl_rect}
    { }

    //! \brief screen_rect ctor
    //! \details Initialize from individual values
    //! \param [in] px x-coordinate
    //! \param [in] py y-coordinate
    //! \param [in] width Width
    //! \param [in] height Height
    constexpr screen_rect (int32 px, int32 py, int32 width, int32 height)
        : SDL_Rect{px, py, width, height}
    { }

    //!@{ Edge values
    constexpr int32 top (void) const noexcept { return y; }
    constexpr int32 left (void) const noexcept { return x; }
    constexpr int32 bottom (void) const noexcept { return y - h; }
    constexpr int32 right (void) const noexcept { return x + w; }
    //!@}

    //!@{ Corner values
    constexpr screen_point top_left (void) const noexcept { return { left(), top() }; }
    constexpr screen_point top_right (void) const noexcept { return { right(), top() }; }
    constexpr screen_point bottom_left (void) const noexcept { return { left(), bottom() }; }
    constexpr screen_point bottom_right (void) const noexcept { return { right(), bottom() }; }
    //!@}
};

//! \brief screen_point equality operator
//! \returns True iff identical
constexpr bool
operator== (const screen_point& lhs, const screen_point& rhs) noexcept
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

//! \brief screen_point inequality operator
//! \returns True iff not identical
constexpr bool
operator!= (const screen_point& lhs, const screen_point& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief screen_rect equality operator
//! \returns True iff identical
constexpr bool
operator== (const screen_rect& lhs, const screen_rect& rhs) noexcept
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.w == rhs.w) && (lhs.h == rhs.h);
}

//! \brief screen_rect inequality operator
//! \returns True iff not identical
constexpr bool
operator!= (const screen_rect& lhs, const screen_rect& rhs) noexcept
{
    return !(lhs == rhs);
}

//!@{ screen_point output and serialization
std::ostream& operator<< (std::ostream&, const screen_point&);
std::string to_string (const screen_point&);
void to_json (nlohmann::json&, const screen_point&);
void from_json (const nlohmann::json&, screen_point&);
//!@}

//!@{ screen_rect output and serialization
std::ostream& operator<< (std::ostream&, const screen_rect&);
std::string to_string (const screen_rect&);
void to_json (nlohmann::json&, const screen_rect&);
void from_json (const nlohmann::json&, screen_rect&);
//!@}

} // namespace rdge
