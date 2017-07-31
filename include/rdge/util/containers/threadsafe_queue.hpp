//! \headerfile <rdge/util/containers/threadsafe_queue.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 07/28/2017

#pragma once

#include <rdge/core.hpp>

#include <queue>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class threadsafe_queue
//! \brief Mutex protected FIFO std::queue
//! \details Queue that allows thread safe read/writes.  The underlying
//!          container is a std::queue which uses a linked list backing store,
//!          and all reads/writes are protected by a mutex.  Use should be
//!          limited to logging/debug tools.
//! \warning AVOID USE IN PERFORMANCE CRITICAL SECTIONS.
template<typename T>
class threadsafe_queue
{
public:
    //!@{ threadsafe_queue default ctor/dtor
    threadsafe_queue (void) = default;
    ~threadsafe_queue (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    threadsafe_queue (const threadsafe_queue&) = delete;
    threadsafe_queue& operator= (const threadsafe_queue&) = delete;

    threadsafe_queue (threadsafe_queue&& rhs) noexcept
    {
        std::lock_guard<std::mutex> guard(rhs.m_mutex);
        m_queue = std::move(rhs.m_queue);
    }

    threadsafe_queue& operator= (threadsafe_queue&& rhs) noexcept
    {
        if (this != &rhs)
        {
            // defer locking to prevent multithread simultaeous move.  e.g.
            // thread a: x = std::move(y);
            // thread b: y = std::move(x);
            std::unique_lock<std::mutex> lhs_lock(m_mutex, std::defer_lock);
            std::unique_lock<std::mutex> rhs_lock(rhs.m_mutex, std::defer_lock);
            std::lock(lhs_lock, rhs_lock);

            m_queue = std::move(rhs.m_queue);
        }

        return *this;
    }
    //!@}

    //! \brief Push an item on the back of the queue
    //! \details Notifies any blocked consumer thread an item is available.
    //! \param [in] item Item to enqueue
    void push (T&& item)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_queue.emplace(std::move(item));
        m_cv.notify_one();
    }

    //! \brief Pop item from the front of the queue
    //! \details Blocks thread until there is an item available.
    //! \param [out] item Front queued item
    void wait_and_pop (T& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [=] {
            return !m_queue.empty();
        });

        item = std::move(m_queue.front());
        m_queue.pop();
    }

    //! \brief Pop item from the front of the queue
    //! \details Blocks thread until there is an item available, or until the
    //!          provided threshold has passed.  False is returned if the call
    //!          failed to provide an item to the caller.
    //! \param [out] item Front queued item
    //! \param [in] ms Maximum wait time in milliseconds
    //! \returns True iff item is popped of the queue, false otherwise
    bool wait_and_pop (T& item, std::chrono::milliseconds ms)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool result = m_cv.wait_for(lock, ms, [=] {
            return !m_queue.empty();
        });

        if (result)
        {
            item = std::move(m_queue.front());
            m_queue.pop();
        }

        return result;
    }

    //! \brief Check if container is empty
    bool empty (void) const
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_queue.empty();
    }

    //! \brief Queue size
    size_t size (void) const
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_queue.size();
    }

    //! \brief Clear container contents
    void clear (void)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::queue<T>().swap(m_queue);
    }

private:
    std::queue<T> m_queue;

    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
};

} // namespace rdge
