//! \headerfile <rdge/util/memory/freelist.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/26/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL_assert.h>

#include <cstdlib>
#include <cstring>
#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class DynamicFreelist
//! \brief Dynamically growing contiguous fixed-block allocator
//! \details Pre-allocated block of memory where an element is accessed by an
//!          integer handle.  The pool maintains a list of 'free' handles to
//!          provide fast reservation/release.  When the pool is exhaused it
//!          will automatically allocate more elements defined by the ChunkSize.
//! \see http://stackoverflow.com/questions/19385853
//! \see https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2011/06/6-1-2010.pdf
template <typename T, size_t ChunkSize = 128>
class DynamicFreelist
{
public:
    // TODO Move to containers
    // TODO Make sure move semantics work w/o the templated type provided.
    //      Also, how should a move be done if they each have a different
    //      ChunkSize

    // TODO Because of the realloc it's easy to have a use after free:
    //        1) Create reference to object
    //        2) Reserve a new spot in the list (realloc occurs)
    //        3) Write to the first object
    //
    //      Bookkeeping could be added so previous allocated memory
    //      is not freed up.  Until then any references retrieved for
    //      writing should have a small scope and never be accessed
    //      after a Reserve() call.  References retrieved for reads
    //      should be const.

    //! \brief DynamicFreelist ctor
    //! \details Allocates heap and initializes the handle list
    //! \param [in] capacity Initial capacity to allocate
    explicit DynamicFreelist (size_t capacity = 0)
    {
        m_capacity = (capacity == 0) ? ChunkSize : capacity;
        m_data = (T*)calloc(m_capacity, sizeof(T));
        m_handles = (uint32*)calloc(m_capacity, sizeof(uint32));

        InitializeHandles();
    }

    //! \brief DynamicFreelist dtor
    //! \details Frees all resources
    ~DynamicFreelist (void) noexcept
    {
        free(m_data);
        free(m_handles);
    }

    //!@{
    //! \brief Non-copyable, move enabled
    DynamicFreelist (const DynamicFreelist&) = delete;
    DynamicFreelist& operator= (const DynamicFreelist&) = delete;

    DynamicFreelist (DynamicFreelist&& rhs) noexcept
        : m_count(rhs.m_count)
        , m_capacity(rhs.m_capacity)
    {
        std::swap(m_data, rhs.m_data);
        std::swap(m_handles, rhs.m_handles);
    }

    DynamicFreelist& operator= (DynamicFreelist&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_data, rhs.m_data);
            std::swap(m_handles, rhs.m_handles);

            m_count = rhs.m_count;
            m_capacity = rhs.m_capacity;
        }

        return *this;
    }
    //!@}

    //! \brief DynamicFreelist Subscript Operator
    //! \details Retrieves the block of data associated to the handle.
    //! \param [in] handle Reserved handle
    //! \returns Reference to the data of the associated handle
    T& operator[] (uint32 handle) const noexcept
    {
        SDL_assert(m_count > 0);
        SDL_assert(handle < m_capacity);
        SDL_assert((ptrdiff_t(&m_handles[handle] - m_handles) / sizeof(uint32)) <= m_count);

        return m_data[handle];
    }

    //! \brief Reserve a block of memory
    //! \returns Handle to the reserved memory
    uint32 Reserve (void)
    {
        if (m_count == m_capacity)
        {
            m_capacity += ChunkSize;
            m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
            m_handles = (uint32*)realloc(m_handles, m_capacity * sizeof(uint32));

            InitializeHandles();
        }

        return m_handles[m_count++];
    }

    //! \brief Releases the block of memory back into the pool
    //! \param [in] handle Reserved handle
    void Release (uint32 handle)
    {
        SDL_assert(m_count > 0);
        SDL_assert(handle < m_capacity);
        SDL_assert((ptrdiff_t(&m_handles[handle] - m_handles) / sizeof(uint32)) <= m_count);

        memset(&m_data[handle], 0, sizeof(T));
        std::swap(m_handles[handle], m_handles[m_count--]);
    }

    //! \returns Number of elements reserved
    size_t Size (void) const noexcept
    {
        return m_count;
    }

    //! \returns Capacity before reallocation
    size_t Capacity (void) const noexcept
    {
        return m_capacity;
    }

private:

    //! \brief Create handles for newly allocated blocks
    void InitializeHandles (void) noexcept
    {
        for (size_t i = m_count; i < m_capacity; ++i)
        {
            m_handles[i] = i;
        }
    }

    T* m_data = nullptr;         //!< Data array
    uint32* m_handles = nullptr; //!< Array of handles that map to indices of the data
    size_t m_count = 0;          //!< Number of stored elements
    size_t m_capacity = 0;       //!< Current array capacity
};

} // namespace rdge
