//! \headerfile <rdge/util/timer.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/24/2015

#pragma once

#include <rdge/core.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace util {

//! \class Timer
//! \brief Timing mechanism
//! \details Timer just stores native types and gets time using
//!          calculations from SDL_GetTicks so copy/moves are supported.
//!          Timer is not threadsafe.
class Timer
{
public:
    //! \brief Timer ctor
    //! \details Sets initial time and starts the timer
    explicit Timer ()
        : m_startTicks(0)
        , m_pausedTicks(0)
        , m_isRunning(false)
        , m_isPaused(false)
    { }

    //! \brief Timer dtor
    ~Timer () { }

    //! \brief Timer Copy ctor
    //! \details Default-copyable
    Timer (const Timer&) = default;

    //! \brief Image Move ctor
    //! \details Default-movable
    Timer (Timer&& rhs) = default;

    //! \brief Timer Copy Assignment Operator
    //! \details Default-copyable
    Timer& operator= (const Timer&) = default;

    //! \brief Image Move Assignment Operator
    //! \details Default-movable
    Timer& operator= (Timer&& rhs) = default;

    //! \brief Start the timer
    void Start (void);

    //! \brief Stop the timer
    void Stop (void);

    //! \brief Pause the timer
    void Pause (void);

    //! \brief Unpause the timer
    void Unpause (void);

    //! \brief Restart the timer
    //! \returns Tick count since last start time
    rdge::uint32 Restart (void);

    //! \brief Get the tick count since the timer started
    //! \returns Tick count since last start time
    rdge::uint32 Ticks (void) const;

    //! \brief Get the tick delta since the last call
    //! \returns Tick count between calls
    rdge::uint32 TickDelta (void);

    //! \brief Get the tick count since the last call to
    //!        TickDelta, without resetting the counter
    //! \returns Tick count since last call to TickDelta
    rdge::uint32 PollTickDelta (void);

    //! \brief Check if the timer is running
    //! \returns True if running, false otherwise
    bool IsRunning (void) const { return m_isRunning; }

    //! \brief Check if the timer is paused
    //! \returns True if running, false otherwise
    bool IsPaused (void) const { return m_isPaused && m_isRunning; }

private:
    rdge::uint32 m_startTicks;
    rdge::uint32 m_pausedTicks;
    rdge::uint32 m_previousTicks;
    bool m_isRunning;
    bool m_isPaused;
};

} // namespace util
} // namespace rdge
