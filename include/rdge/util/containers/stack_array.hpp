//! \headerfile <rdge/util/containers/stack_array.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 06/27/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/containers/iterators.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>

#include <SDL_assert.h>

#include <utility>
#include <stdexcept>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct stack_array
//! \brief POD sequence container specialized for repeated stack reuse
//! \details Variable sized array useful to populate small scoped data in order
//!          to benefit from the cache-locality to operate on said data.
//!          The container dynamically allocates memory only when the reserved
//!          size exceeds the capacity.  In that way behavior is similar to a
//!          std::vector, but has the added benefit of avoiding the uneccessary
//!          push_back copy.
template <typename T>
struct stack_array
{
    using iterator = detail::ra_iterator<T>;

    //! \brief Size multiplier when a realloc is required
    static constexpr float OVER_ALLOC_RATIO = 1.5f;

    //! \brief stack_array default ctor
    explicit stack_array (size_t capacity = 0)
        : m_capacity(capacity)
    {
        if (RDGE_UNLIKELY(!RDGE_TMALLOC(m_data, m_capacity, memory_bucket_containers)))
        {
            throw std::runtime_error("Memory allocation failed");
        }
    }

    //! \brief stack_array dtor
    ~stack_array (void) noexcept
    {
        RDGE_FREE(m_data, memory_bucket_containers);
    }

    //!@{ Non-copyable, move enabled
    stack_array (const stack_array&) = delete;
    stack_array& operator= (const stack_array&) = delete;

    stack_array (stack_array&& rhs) noexcept
        : m_count(rhs.m_count)
        , m_capacity(rhs.m_capacity)
    {
        std::swap(m_data, rhs.m_data);
    }

    stack_array& operator= (stack_array&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_data, rhs.m_data);

            m_count = rhs.m_count;
            m_capacity = rhs.m_capacity;
        }

        return *this;
    }
    //!@}

    //!@{ Random access iterator support
    iterator begin (void) { return iterator(&m_data[0]); }
    iterator end (void) { return iterator(&m_data[m_count]); }
    //!@}

    //! \brief stack_array Subscript Operator
    //! \returns Reference to the data at the provided index
    T& operator[] (uint32 index) const noexcept
    {
        SDL_assert(m_data);
        SDL_assert(m_count > 0);
        SDL_assert(index < m_count);

        return m_data[index];
    }

    //! \brief Get the next free element
    //! \details Increments the size of the container
    //! \returns Reference to the data at the next available index
    T& next (void) noexcept
    {
        SDL_assert(m_data);
        SDL_assert(m_capacity > 0);
        SDL_assert(m_count < m_capacity);

        return m_data[m_count++];
    }

    //! \brief Get the next free element initialized to zero
    //! \details Increments the size of the container
    //! \returns Reference to the data at the next available index
    T& next_clean (void) noexcept
    {
        SDL_assert(m_data);
        SDL_assert(m_capacity > 0);
        SDL_assert(m_count < m_capacity);

        memset(m_data + m_count, 0, sizeof(T));
        return m_data[m_count++];
    }

    //! \brief Reserve a number of elements
    //! \details Call does nothing if the requested capacity is less than
    //!          the current capacity.
    //! \param [in] new_cap Capacity requested
    void reserve (size_t new_cap)
    {
        if (new_cap > m_capacity)
        {
            m_capacity = (static_cast<float>(new_cap) * OVER_ALLOC_RATIO);
            if (RDGE_UNLIKELY(!RDGE_TREALLOC(m_data, m_capacity, memory_bucket_containers)))
            {
                throw std::runtime_error("Memory allocation failed");
            }
        }
    }

    //! \brief Clear contained elements
    void clear (void) noexcept
    {
        m_count = 0;
    }

    //!@{ Container properties
    bool empty (void) const noexcept { return (m_count == 0); }
    size_t size (void) const noexcept { return m_count; }
    size_t capacity (void) const noexcept { return m_capacity; }
    //!@}

private:
    T* m_data = nullptr;   //!< Data array
    size_t m_count = 0;    //!< Number of stored elements
    size_t m_capacity = 0; //!< Current array capacity
};

} // namespace rdge
