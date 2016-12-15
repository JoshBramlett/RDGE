#pragma once

#include <queue>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

namespace rdge {
namespace util {

template<typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue (void) { }

    ThreadsafeQueue (const ThreadsafeQueue&) = delete;

    ThreadsafeQueue (ThreadsafeQueue&& rhs) noexcept
    {
        std::lock_guard<std::mutex> lock(rhs.m_mtx);
        m_data = std::move(rhs.m_data);
    }

    ThreadsafeQueue& operator= (const ThreadsafeQueue&) = delete;

    ThreadsafeQueue& operator= (ThreadsafeQueue&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::lock_guard<std::mutex> lock(rhs.m_mtx);
            m_data = std::move(rhs.m_data);
        }

        return *this;
    }

    ~ThreadsafeQueue (void) = default;

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

    bool Empty (void) const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_data.empty();
    }

    size_t Size(void) const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_data.size();
    }

    void Clear (void)
    {
        std::queue<T> temp;
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.swap(temp);
    }

private:
    mutable std::mutex m_mtx;
    std::queue<T> m_data;
    std::condition_variable m_condition;
};

} // namespace util
} // namespace rdge
