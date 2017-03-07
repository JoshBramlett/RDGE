#include <rdge/util/timer.hpp>

#include <SDL.h>

namespace rdge {

void
Timer::Start (void) noexcept
{
    m_isRunning = true;
    m_isPaused = false;
    m_startTicks = SDL_GetTicks();
    m_deltaTicks = m_startTicks;
    m_pausedTicks = 0;
}

void
Timer::Stop (void) noexcept
{
    m_isRunning = false;
    m_isPaused = false;
    m_startTicks = 0;
    m_pausedTicks = 0;
    m_deltaTicks = 0;
}

void
Timer::Pause (void) noexcept
{
    if (m_isRunning && !m_isPaused)
    {
        m_isPaused = true;
        m_pausedTicks = SDL_GetTicks() - m_startTicks;
        m_deltaTicks = 0;
    }
}

void
Timer::Resume (void) noexcept
{
    if (m_isRunning && m_isPaused)
    {
        m_isPaused = false;
        m_startTicks += SDL_GetTicks() - m_pausedTicks;
        m_deltaTicks = m_startTicks;
        m_pausedTicks = 0;
    }
}

uint32
Timer::Restart (void) noexcept
{
    uint32 ticks = Ticks();
    Start();

    return ticks;
}

uint32
Timer::Ticks (void) const noexcept
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

uint32
Timer::TickDelta (void) noexcept
{
    if (m_isRunning && !m_isPaused)
    {
        uint32 ticks = SDL_GetTicks();
        uint32 delta = ticks - m_deltaTicks;
        m_deltaTicks = ticks;

        return delta;
    }

    return 0;
}

uint32
Timer::PollTickDelta (void) const noexcept
{
    if (m_isRunning && !m_isPaused)
    {
        return SDL_GetTicks() - m_deltaTicks;
    }

    return 0;
}

} // namespace rdge
