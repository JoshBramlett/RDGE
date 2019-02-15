//! \headerfile <rdge/util/adt/simple_varray.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 02/14/2019

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/debug/assert.hpp>

#include <type_traits>
#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct simple_varray
//! \brief Variable sized sequence container
//! \details Variable fixed sized array specialized for piecemeal assignment of it's elements.
//!          Elements are default constructed on initialization and values can be assigned
//!          through the random access accessors.  It's main utility is in deserialization.
template <typename T, memory_bucket Bucket = memory_bucket_containers>
struct simple_varray
{
    static_assert(std::is_default_constructible<T>::value,
                  "simple_varray requires types to be default constructable");

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //! \brief simple_varray default ctor
    simple_varray (void) = default;

    //! \brief simple_varray capacity ctor
    //! \param [in] capacity Total (immutable) capacity of the array
    explicit simple_varray (size_t capacity)
        : m_capacity(capacity)
    {
        if (RDGE_UNLIKELY(!RDGE_TMALLOC(m_data, m_capacity, Bucket)))
        {
            RDGE_THROW_ALLOC_FAILED();
        }

        for (size_t i = 0; i < m_capacity; i++)
        {
            new (m_data + i) T();
        }
    }

    //! \brief simple_varray dtor
    ~simple_varray (void) noexcept
    {
        RDGE_FREE(m_data, Bucket);
    }

    //!@{ Non-copyable, move enabled
    simple_varray (const simple_varray&) = delete;
    simple_varray& operator= (const simple_varray&) = delete;

    simple_varray (simple_varray&& rhs) noexcept
        : m_data(rhs.m_data)
        , m_capacity(rhs.m_capacity)
    {
        rhs.m_data = nullptr;
    }

    simple_varray& operator= (simple_varray&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_data, rhs.m_data);
            m_capacity = rhs.m_capacity;
        }

        return *this;
    }
    //!@}

    //!@{ Random access iterator support
    RDGE_ALWAYS_INLINE iterator begin (void) { return iterator(m_data); }
    RDGE_ALWAYS_INLINE const_iterator begin (void) const { return const_iterator(m_data); }
    RDGE_ALWAYS_INLINE const_iterator cbegin (void) const { return const_iterator(m_data); }
    RDGE_ALWAYS_INLINE iterator end (void) { return iterator(m_data + m_capacity); }
    RDGE_ALWAYS_INLINE const_iterator end (void) const { return const_iterator(m_data + m_capacity); }
    RDGE_ALWAYS_INLINE const_iterator cend (void) const { return const_iterator(m_data + m_capacity); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
    //!@}

    //!@{ simple_varray Subscript Operators
    reference operator[] (size_t index) noexcept
    {
        RDGE_ASSERT(m_data);
        RDGE_ASSERT(m_capacity > 0);
        RDGE_ASSERT(index < m_capacity);

        return m_data[index];
    }

    const_reference operator[] (size_t index) const noexcept
    {
        RDGE_ASSERT(m_data);
        RDGE_ASSERT(m_capacity > 0);
        RDGE_ASSERT(index < m_capacity);

        return m_data[index];
    }
    //!@}

    //! /brief Element access with bounds checking
    //! /throws std::out_of_range
    reference at (size_t index)
    {
        RDGE_ASSERT(m_data);
        if (index < m_capacity)
        {
            return m_data[index];
        }

        throw std::out_of_range("Index out of range");
    }

    //! /brief Const element access with bounds checking
    //! /throws std::out_of_range
    const_reference at (size_t index) const
    {
        RDGE_ASSERT(m_data);
        if (index < m_capacity)
        {
            return m_data[index];
        }

        throw std::out_of_range("Index out of range");
    }

    //!@{ Container properties
    RDGE_ALWAYS_INLINE size_t size (void) const noexcept { return m_capacity; }
    RDGE_ALWAYS_INLINE size_t capacity (void) const noexcept { return m_capacity; }
    RDGE_ALWAYS_INLINE bool empty (void) const noexcept { return (m_capacity == 0); }
    //!@}

private:
    T* m_data = nullptr;   //!< Data array
    size_t m_capacity = 0; //!< Current array capacity
};

} // namespace rdge
