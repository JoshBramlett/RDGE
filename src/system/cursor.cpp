#include <rdge/system/cursor.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL.h>

using namespace rdge;

namespace {
    // The global cursor is static, and is used for the purpose
    // of not having the overhead of changing cursors if not required.
    SystemCursor s_globalSystemCursor = SystemCursor::Arrow;
    std::string  s_globalCustomCursor = "";
}

Cursor::~Cursor (void)
{
    for (const auto& cursor : m_systemCursors)
    {
        SDL_FreeCursor(cursor.second);
    }

    for (const auto& cursor : m_systemCursors)
    {
        SDL_FreeCursor(cursor.second);
    }
}

Cursor::Cursor (Cursor&& rhs) noexcept
{
    m_systemCursors.swap(rhs.m_systemCursors);
    m_customCursors.swap(rhs.m_customCursors);
}

Cursor&
Cursor::operator= (Cursor&& rhs) noexcept
{
    if (this != &rhs)
    {
        for (const auto& cursor : m_systemCursors)
        {
            SDL_FreeCursor(cursor.second);
        }

        for (const auto& cursor : m_customCursors)
        {
            SDL_FreeCursor(cursor.second);
        }

        m_systemCursors.swap(rhs.m_systemCursors);
        m_customCursors.swap(rhs.m_customCursors);
    }

    return *this;
}

void
Cursor::CreateCustomCursor (
                            const std::string&,
                            const std::string&,
                            rdge::int32,
                            rdge::int32
                           )
{
    // TODO: [00027] Implement SDL_CreateColorCursor
    //       Make sure ID is not an empty string
    RDGE_THROW("Functionality not implemented");
}

void
Cursor::Set (SystemCursor cursor)
{
    if (cursor == SystemCursor::NotSet)
    {
        RDGE_THROW("SystemCursor cannot be set to NotSet");
    }

    if (cursor == s_globalSystemCursor)
    {
        return;
    }

    SDL_Cursor* sdl_cursor = nullptr;
    auto iter = m_systemCursors.find(cursor);
    if (iter == m_systemCursors.end())
    {
        sdl_cursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor));
        m_systemCursors[cursor] = sdl_cursor;
    }
    else
    {
        sdl_cursor = iter->second;
    }

    if (UNLIKELY(sdl_cursor == nullptr))
    {
        RDGE_THROW("Cannot find system cursor");
    }

    // set the previous state for revert
    m_previousSystemCursor = s_globalSystemCursor;
    m_previousCustomCursor = s_globalCustomCursor;

    // set the local and global state
    m_currentSystemCursor = s_globalSystemCursor = cursor;
    m_currentCustomCursor = s_globalCustomCursor = "";

    SDL_SetCursor(sdl_cursor);
}

void
Cursor::Set (const std::string&)
{
    // TODO: [00027] Implement SDL_CreateColorCursor
    RDGE_THROW("Functionality not implemented");
}

void
Cursor::Revert (void)
{
    if (m_previousSystemCursor == SystemCursor::NotSet)
    {
        if (m_currentCustomCursor != s_globalCustomCursor)
        {
            return;
        }

        Set (m_previousCustomCursor);
    }
    else if (m_previousCustomCursor == "")
    {
        if (m_currentSystemCursor != s_globalSystemCursor)
        {
            return;
        }

        Set (m_previousSystemCursor);
    }
}

void
Cursor::Hide (void)
{
    SDL_ShowCursor(0);
}

void
Cursor::Show (void)
{
    SDL_ShowCursor(1);
}
