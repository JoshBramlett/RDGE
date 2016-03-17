#pragma once

#include <queue>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

namespace RDGE {
namespace Util {

template<typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue () { }

    ThreadsafeQueue (const ThreadsafeQueue& rhs) noexcept
    {
        std::lock_guard<std::mutex> lock(rhs.m_mtx);
        m_data = rhs.m_data;
    }

    ThreadsafeQueue (ThreadsafeQueue&&) = delete;

    ThreadsafeQueue& operator= (const ThreadsafeQueue& rhs)
    {
        std::lock_guard<std::mutex> lock(rhs.m_mtx);
        m_data = rhs.m_data;
    }

    ThreadsafeQueue& operator= (ThreadsafeQueue&&) = delete;

    void Push (T&& item)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.emplace(std::move(item));
        m_condition.notify_one();
    }

    void WaitAndPop (T& item)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_condition.wait(lock, [this] {
            return !m_data.empty();
        });

        item = std::move(m_data.front());
        m_data.pop();
    }

    bool WaitAndPop (T& item, unsigned int ms)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        bool rt = m_condition.wait_for(lock, std::chrono::milliseconds(ms), [this] {
            return !m_data.empty();
        });

        if (rt)
        {
            item = std::move(m_data.front());
            m_data.pop();
        }

        return rt;
    }

    bool Empty () const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_data.empty();
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_data.size();
    }

    void Clear ()
    {
        std::queue<T> temp;
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.swap(temp);
    }

    ~ThreadsafeQueue () = default;

private:
    mutable std::mutex m_mtx;
    std::queue<T> m_data;
    std::condition_variable m_condition;
};

} // namespace Util
} // namespace RDGE
