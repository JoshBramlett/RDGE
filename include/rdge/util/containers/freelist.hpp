//! \headerfile <rdge/util/containers/freelist.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/17/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <SDL_assert.h>

#include <utility>
#include <stdexcept>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct freelist
//! \brief Dynamically growing contiguous fixed-block allocator
//! \details Pre-allocated block of memory where an element is accessed by an
//!          integer handle.  The pool maintains a list of 'free' handles to
//!          provide fast reservation/release.  When the pool is exhaused it
//!          will automatically allocate more elements defined by the ChunkSize.
//! \warning Accessing an element should have an extremely small scope.  If
//!          a reallocation occurs while a reference is still in scope that
//!          reference will become invalid.  It's recommended to assign to a
//!          const reference variable whenever possible.
//! \see http://stackoverflow.com/questions/19385853
//! \see https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2011/06/6-1-2010.pdf
template <typename T, size_t ChunkSize = 128>
struct freelist
{
public:
    // TODO Because of the realloc it's easy to have a use after free:
    //        1) Create reference to object
    //        2) Reserve a new spot in the list (realloc occurs)
    //        3) Write to the first object
    //      Bookkeeping could be added so previous allocated memory
    //      is not freed up.  See warning.

    //! \brief freelist ctor
    //! \details Allocates heap and initializes the handle list
    //! \param [in] capacity Initial capacity to allocate
    //! \throws std::runtime_error Memory allocation failed
    explicit freelist (size_t capacity = 0)
        : m_capacity((capacity == 0) ? ChunkSize : capacity)
    {
        TRACK_MEMORY(this->mem_prof);
        if (!RDGE_CALLOC(m_data, m_capacity, &this->mem_prof))
        {
            throw std::runtime_error("Failed to allocate memory");
        }

        if (!RDGE_CALLOC(m_handles, m_capacity, &this->mem_prof))
        {
            throw std::runtime_error("Failed to allocate memory");
        }

        create_handles();
    }

    //! \brief freelist dtor
    //! \details Frees all resources
    ~freelist (void) noexcept
    {
        RDGE_FREE(m_data, &this->mem_prof);
        RDGE_FREE(m_handles, &this->mem_prof);
        UNTRACK_MEMORY(this->mem_prof);
    }

    //!@{ Non-copyable, move enabled
    freelist (const freelist&) = delete;
    freelist& operator= (const freelist&) = delete;

    freelist (freelist&& rhs) noexcept
        : m_count(rhs.m_count)
        , m_capacity(rhs.m_capacity)
    {
        std::swap(m_data, rhs.m_data);
        std::swap(m_handles, rhs.m_handles);

#ifdef RDGE_DEBUG
        std::swap(mem_prof, rhs.mem_prof);
#endif
    }

    freelist& operator= (freelist&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_data, rhs.m_data);
            std::swap(m_handles, rhs.m_handles);

#ifdef RDGE_DEBUG
            std::swap(mem_prof, rhs.mem_prof);
#endif
            m_count = rhs.m_count;
            m_capacity = rhs.m_capacity;
        }

        return *this;
    }
    //!@}

    //! \brief freelist Subscript Operator
    //! \details Retrieves the block of data associated to the handle.
    //! \param [in] handle Reserved handle
    //! \returns Reference to the data of the associated handle
    T& operator[] (uint32 handle) const noexcept
    {
        SDL_assert(m_count > 0);
        SDL_assert(handle < m_capacity);
        SDL_assert(is_reserved(handle));

        return m_data[handle];
    }

    //! \brief Reserve a block of memory
    //! \returns Handle to the reserved memory
    //! \throws std::runtime_error Memory allocation failed
    uint32 reserve (void)
    {
        if (m_count == m_capacity)
        {
            m_capacity += ChunkSize;
            if (!RDGE_REALLOC(m_data, m_capacity, &this->mem_prof))
            {
                throw std::runtime_error("Failed to allocate memory");
            }

            if (!RDGE_REALLOC(m_handles, m_capacity, &this->mem_prof))
            {
                throw std::runtime_error("Failed to allocate memory");
            }

            create_handles();
        }

        return m_handles[m_count++];
    }

    //! \brief Releases the block of memory back into the pool
    //! \param [in] handle Reserved handle
    void release (uint32 handle)
    {
        SDL_assert(m_count > 0);
        SDL_assert(handle < m_capacity);
        SDL_assert(is_reserved(handle));

        memset(&m_data[handle], 0, sizeof(T));
        for (size_t i = 0; i < m_count; i++)
        {
            if (m_handles[i] == handle)
            {
                std::swap(m_handles[i], m_handles[--m_count]);
                return;
            }
        }
    }

    //! \returns True iff handle is in the reserved list
    bool is_reserved (uint32 handle) const noexcept
    {
        for (size_t i = 0; i < m_count; i++)
        {
            if (m_handles[i] == handle)
            {
                return true;
            }
        }

        return false;
    }

    //!@{ Container properties
    bool empty (void) const noexcept { return (m_count == 0); }
    size_t size (void) const noexcept { return m_count; }
    size_t capacity (void) const noexcept { return m_capacity; }
    //!@}

private:

    //! \brief Create handles for newly allocated blocks
    void create_handles (void) noexcept
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

#ifdef RDGE_DEBUG
public:
    memory_profile mem_prof;
#endif
};

} // namespace rdge
