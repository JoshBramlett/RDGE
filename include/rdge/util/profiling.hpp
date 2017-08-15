//! \headerfile <rdge/util/profiling.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/14/2017

#pragma once

#include <rdge/core.hpp>

#include <chrono>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class ScopeProfiler
//! \brief Stores elapsed time between construction and destruction
//! \details Used for profiling, a high resolution time point is recorded on
//!          object creation and destruction, whose value is stored in the
//!          provided variable
template <typename Duration = std::chrono::microseconds>
class ScopeProfiler
{
public:
    using Clock = std::chrono::high_resolution_clock;

    //! \brief ScopeProfiler ctor
    //! \details Records a starting time point.
    //! \param [in] var Variable to store the elapsed time
    explicit ScopeProfiler (typename Duration::rep* var)
        : m_store(var)
        , m_start(std::chrono::time_point_cast<Duration>(Clock::now()))
    { }

    //! \brief ScopeProfiler dtor
    //! \details Records an ending time point and stores the delta.
    ~ScopeProfiler (void) noexcept
    {
        auto end = std::chrono::time_point_cast<Duration>(Clock::now());
        *m_store = std::chrono::duration_cast<Duration>(end - m_start).count();
    }

    //!@{ Copy/Move disabled
    ScopeProfiler (const ScopeProfiler&) = delete;
    ScopeProfiler& operator= (const ScopeProfiler&) = delete;
    ScopeProfiler (ScopeProfiler&&) = delete;
    ScopeProfiler& operator= (ScopeProfiler&&) = delete;
    //!@}

private:
    typename Duration::rep* m_store = nullptr;
    std::chrono::time_point<std::chrono::high_resolution_clock, Duration> m_start;
};

} // namespace rdge
