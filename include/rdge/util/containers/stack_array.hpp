//! \headerfile <rdge/util/containers/stack_array.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 06/27/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/containers/iterators.hpp>

#include <SDL_assert.h>

#include <cstdlib>
#include <cstring>
#include <utility>
#include <algorithm>

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
    static constexpr float NOISE_MULTIPLIER = 1.5f; //!< Amount to over-allocate

    //! \brief stack_array default ctor
    stack_array (void) = default;

    //! \brief stack_array ctor
    explicit stack_array (size_t capacity)
        : m_capacity(capacity)
    {
        m_data = (T*)malloc(m_capacity * sizeof(T));
    }

    //! \brief stack_array dtor
    ~stack_array (void) noexcept
    {
        free(m_data);
    }

    //!@{
    //! \brief Non-copyable, move enabled
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
            free(m_data);

            m_capacity = (static_cast<float>(new_cap) * NOISE_MULTIPLIER);
            m_data = (T*)malloc(m_capacity * sizeof(T));
        }
    }

    //! \brief Clear contained elements
    void clear (void)
    {
        m_count = 0;
    }

    //! \returns True iff no elements are contained
    bool empty (void) const noexcept { return (m_count == 0); }

    //! \returns Number of contained elements
    size_t size (void) const noexcept { return m_count; }

    //! \returns Number of allocated elements
    size_t capacity (void) const noexcept { return m_capacity; }

    //!@{ Random access iterator support
    using iterator = detail::ra_iterator<T>;
    iterator begin (void) { return iterator(&m_data[0]); }
    iterator end (void) { return iterator(&m_data[m_count]); }
    //!@}

private:
    T* m_data = nullptr;   //!< Data array
    size_t m_count = 0;    //!< Number of stored elements
    size_t m_capacity = 0; //!< Current array capacity
};

} // namespace rdge
