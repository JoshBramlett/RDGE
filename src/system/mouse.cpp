#include <rdge/system/mouse.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <sstream>

namespace rdge {

Cursor::Cursor (SystemCursor cursor)
{
    m_cursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor));
    if (UNLIKELY(!m_cursor))
    {
        std::ostringstream ss;
        ss << "Failed to load system cursor"
           << " cursor_id=" << static_cast<int32>(cursor);

        SDL_THROW(ss.str(), "SDL_CreateSystemCursor");
    }
}

Cursor::Cursor (Surface& surface, int32 hot_x, int32 hot_y)
{
    m_cursor = SDL_CreateColorCursor(static_cast<SDL_Surface*>(surface), hot_x, hot_y);
    if (UNLIKELY(!m_cursor))
    {
        SDL_THROW("Failed to load custom cursor", "SDL_CreateColorCursor");
    }
}

Cursor::Cursor (Surface&& surface, int32 hot_x, int32 hot_y)
    : Cursor(surface, hot_x, hot_y)
{ }

Cursor::~Cursor (void) noexcept
{
    if (m_cursor)
    {
        SDL_FreeCursor(m_cursor);
    }
}

Cursor::Cursor (Cursor&& rhs) noexcept
{
    if (m_cursor)
    {
        SDL_FreeCursor(m_cursor);
    }

    m_cursor = rhs.m_cursor;
    rhs.m_cursor = nullptr;
}

Cursor&
Cursor::operator= (Cursor&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_cursor)
        {
            SDL_FreeCursor(m_cursor);
        }

        m_cursor = rhs.m_cursor;
        rhs.m_cursor = nullptr;
    }

    return *this;
}

bool
IsCursorHidden (void) noexcept
{
    return (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE);
}

void
ShowCursor (void) noexcept
{
    SDL_ShowCursor(SDL_ENABLE);
}

void
HideCursor (void) noexcept
{
    SDL_ShowCursor(SDL_DISABLE);
}

void
SetCursor (const Cursor& cursor) noexcept
{
    // NOTE Passing a nullptr to SDL_SetCursor is valid, and forces the cursor
    //      to be redrawn.  If redrawing a cursor is desired later on we can add
    //      a new function for that.
    auto cursor_ptr = static_cast<SDL_Cursor*>(cursor);
    SDL_assert(cursor_ptr != nullptr);

    SDL_SetCursor(static_cast<SDL_Cursor*>(cursor));
}

void
SetDefaultCursor (void) noexcept
{
    auto cursor = SDL_GetDefaultCursor();
    if (UNLIKELY(!cursor))
    {
        WLOG("SDL_GetDefaultCursor returned a nullptr");
    }
    else
    {
        SDL_SetCursor(cursor);
    }
}

std::ostream& operator<< (std::ostream& os, MouseButton button)
{
    switch (button)
    {
#define CASE(X) case X: os << (strrchr(#X, ':') + 1); break;
        CASE(MouseButton::UNKNOWN)
        CASE(MouseButton::LEFT)
        CASE(MouseButton::MIDDLE)
        CASE(MouseButton::RIGHT)
        CASE(MouseButton::X1)
        CASE(MouseButton::X2)
#undef CASE
        default:
            os << "NOT_FOUND[" << static_cast<uint32>(button) << "]";
    }

    return os;
}

std::ostream& operator<< (std::ostream& os, SystemCursor cursor)
{
    switch (cursor)
    {
#define CASE(X) case X: os << (strrchr(#X, ':') + 1); break;
        CASE(SystemCursor::ARROW)
        CASE(SystemCursor::IBEAM)
        CASE(SystemCursor::WAIT)
        CASE(SystemCursor::CROSSHAIR)
        CASE(SystemCursor::WAITARROW)
        CASE(SystemCursor::SIZENWSE)
        CASE(SystemCursor::SIZENESW)
        CASE(SystemCursor::SIZEWE)
        CASE(SystemCursor::SIZENS)
        CASE(SystemCursor::SIZEALL)
        CASE(SystemCursor::NO)
        CASE(SystemCursor::HAND)
#undef CASE
        default:
            os << "NOT_FOUND[" << static_cast<uint32>(cursor) << "]";
    }

    return os;
}

} // namespace rdge
