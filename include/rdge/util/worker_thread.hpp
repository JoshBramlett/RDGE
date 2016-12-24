#pragma once

#include <thread>
#include <functional>

namespace rdge {
namespace util {

class WorkerThread
{
public:
    WorkerThread () { }

    WorkerThread (
                  std::function<void()> work_func,
                  std::function<void()> stop_func = nullptr
                 )
        : m_stopFunc(stop_func)
    {
        m_thread = std::thread(work_func);
    }

    WorkerThread (const WorkerThread&) = delete;

    WorkerThread (WorkerThread&& rhs) noexcept
        : m_thread(std::move(rhs.m_thread))
        , m_stopFunc(rhs.m_stopFunc)
    { }

    WorkerThread& operator= (const WorkerThread&) = delete;

    WorkerThread& operator= (WorkerThread&& rhs) noexcept
    {
        if (this != &rhs)
        {
            m_thread = std::move(rhs.m_thread);
            m_stopFunc = rhs.m_stopFunc;
        }

        return *this;
    }

    virtual ~WorkerThread ()
    {
        Stop();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void Stop ()
    {
        if (m_stopFunc != nullptr)
        {
            m_stopFunc();
        }
    }

protected:
    std::thread m_thread;
    std::function<void()> m_stopFunc;
};


} // namespace util
} // namespace rdge
