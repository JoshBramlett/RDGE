//! \headerfile <rdge/util/timer.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/24/2015

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Timer
//! \brief Timing mechanism
//! \details The most common practice is for querying \ref TickDelta every frame
//!          to receive the number of ticks (in milliseconds) since the last call.
//! \note Timer is not threadsafe.
class Timer
{
public:
    //! \brief Timer ctor
    Timer (void) = default;

    void Start (void) noexcept;  //!< Start the timer
    void Stop (void) noexcept;   //!< Stop the timer
    void Pause (void) noexcept;  //!< Pause the timer
    void Resume (void) noexcept; //!< Resume the timer

    //! \brief Restart the timer
    //! \returns Tick count since last start time
    uint32 Restart (void) noexcept;

    //! \brief Ticks since the timer started
    //! \returns Tick count since last start time
    uint32 Ticks (void) const noexcept;

    //! \brief Ticks since last call to this function
    //! \note The first call returns the delta from when the timer was started.
    //! \returns Tick count between calls
    uint32 TickDelta (void) noexcept;

    //! \brief Ticks since the last call to \ref TickDelta without resetting
    //! \returns Tick count since last call to \ref TickDelta
    uint32 PollTickDelta (void) const noexcept;

    //! \brief Check if the timer is running
    //! \returns True if running, false otherwise
    bool IsRunning (void) const noexcept { return m_isRunning; }

    //! \brief Check if the timer is paused
    //! \returns True if paused, false otherwise
    bool IsPaused (void) const noexcept { return m_isPaused && m_isRunning; }

private:
    uint32 m_startTicks  = 0; //!< Ticks when the timer was started
    uint32 m_pausedTicks = 0; //!< Ticks when the timer was paused
    uint32 m_deltaTicks  = 0; //!< Ticks for the last delta call

    bool m_isRunning = false; //!< Timer running flag
    bool m_isPaused  = false; //!< Timer paused flag
};

} // namespace rdge
