#include <rdge/util/timer.hpp>

#include <SDL.h>

//! \namespace RDGE: Rainbow Drop Game Engine
namespace RDGE {
namespace Util {

void
Timer::Start (void)
{
    m_isRunning = true;
    m_isPaused = false;
    m_startTicks = m_previousTicks = SDL_GetTicks();
    m_pausedTicks = 0;
}

void
Timer::Stop (void)
{
    m_isRunning = false;
    m_isPaused = false;
    m_startTicks = 0;
    m_pausedTicks = 0;
    m_previousTicks = 0;
}

void
Timer::Pause (void)
{
    if (m_isRunning && !m_isPaused)
    {
        m_isPaused = true;
        m_pausedTicks = SDL_GetTicks() - m_startTicks;
        m_startTicks = 0;
        m_previousTicks = 0;
    }
}

void
Timer::Unpause (void)
{
    if (m_isRunning && m_isPaused)
    {
        m_isPaused = false;
        m_startTicks = m_previousTicks = SDL_GetTicks() - m_pausedTicks;
        m_pausedTicks = 0;
    }
}

RDGE::UInt32
Timer::Restart (void)
{
    auto ticks = Ticks();
    Start();
    return ticks;
}

RDGE::UInt32
Timer::Ticks (void) const
{
    if (m_isRunning)
    {
        if (m_isPaused)
        {
            return m_pausedTicks;
        }

        return SDL_GetTicks() - m_startTicks;
    }

    return 0;
}

RDGE::UInt32
Timer::TickDelta (void)
{
    if (m_isRunning && !m_isPaused)
    {
        auto ticks = SDL_GetTicks();
        auto delta = ticks - m_previousTicks;
        m_previousTicks = ticks;
        return delta;
    }

    return 0;
}

RDGE::UInt32
Timer::PollTickDelta (void)
{
    if (m_isRunning && !m_isPaused)
    {
        return SDL_GetTicks() - m_previousTicks;
    }

    return 0;
}

} // namespace Util
} // namespace RDGE
