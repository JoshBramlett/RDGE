//! \headerfile <rdge/util/memory/small_block_allocator.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/27/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <array>
#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class SmallBlockAllocator
//! \brief Dynamically allocates blocks of memory for small objects
//! \details Attempts to limit the number of allocations by creating chunks
//!          of memory upfront.  Memory released is not freed and will be used
//!          to fulfill future requests.  Utility comes when requests are small,
//!          but it can be used as a general purpose allocator as requests
//!          larger than \ref MAX_BLOCK_SIZE are dynamically performed.
//!
//!          The allocator contains a predefined table of block sizes. When an
//!          allocation request is made, if there is no chunk pre-allocated a
//!          dynamic allocation of of \ref CHUNK_SIZE is performed.  That chunk
//!          is then broken down into block sized pieces which may be assigned
//!          to subsequent requests.  When the chunk has been exhausted a new
//!          dynamic allocation will take place.
//!
//!          Cache locality is generally rather poor.  Drawbacks include:
//!            1) Allocation requests are assigned to the smallest block that
//!               can fill the request, but the difference between the block
//!               size and request size is dead memory.
//!            2) Because chunks are not shared by different block sizes it's
//!               likely multiple requests of the same object may have cache
//!               locality.  However, because preference is given to freed
//!               blocks when fulfilling an allocation request, as more requests
//!               are facilitated it becomes more and more likely multiple
//!               requests for the same object will not be near one another
//!               in memory, and may even span different chunks.
//! \warning Memory may not be zero initialized
//! \note Inspired by b2BlockAllocator in Box2D
//! \see https://github.com/erincatto/Box2D
class SmallBlockAllocator
{
public:
    // TODO If alignment requirements are needed, class could include a default
    //      template value (defaults to 16) specifying the alignment.
    //      windows: _aligned_malloc/_aligned_free
    //      mac/linux: posix_memalign/free
    //
    // TODO Add unit tests
    // TODO It'd be good to get benchmarking results for a real use case

    static constexpr size_t CHUNK_SIZE = 16 * 1024; //!< Size of each heap allocation
    static constexpr size_t CHUNK_ELEMENTS = 128;   //!< Number of chunks allocated
    static constexpr size_t MAX_BLOCK_SIZE = 640;   //!< Maximum supported block size
    static constexpr size_t NUM_BLOCK_SIZES = 14;   //!< Count of supported sizes

    //! \brief SmallBlockAllocator ctor
    //! \details Initializes heap list
    SmallBlockAllocator (void);

    //! \brief SmallBlockAllocator dtor
    //! \details Frees all resources
    ~SmallBlockAllocator (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    SmallBlockAllocator (const SmallBlockAllocator&) = delete;
    SmallBlockAllocator& operator= (const SmallBlockAllocator&) = delete;
    SmallBlockAllocator (SmallBlockAllocator&&) noexcept;
    SmallBlockAllocator& operator= (SmallBlockAllocator&&) noexcept;
    //!@}

    //! \brief Get an allocated block of memory
    //! \details The block of memory allocated may be larger than requested
    //! \param [in] size Size of memory in bytes (must be larger than zero)
    //! \returns Opaque pointer to allocated memory
    void* Alloc (size_t size);

    //! \brief Get an allocated block of memory
    //! \details Explicit type overload of opaque Alloc.
    //! \returns Type casted pointer to allocated memory
    template <typename T>
    T* Alloc (void)
    {
        return reinterpret_cast<T*>(Alloc(sizeof(T)));
    }

    //! \brief Perform new initialization on an allocated block of memory
    //! \details Uses placement new with the allocated block for initialization,
    //!          forwarding any parameters provided.
    //! \returns Type casted pointer to an initialized object
    template <typename T, typename... U>
    T* New (U&&... u)
    {
        void* cursor = Alloc(sizeof(T));
        return new (cursor) T(std::forward<U>(u)...);
    }

    //! \brief Release ownership of the allocated memory
    //! \warning Pointer must be allocated from this instance
    //! \param [in] p Opaque pointer to free
    //! \param [in] size Size of memory in bytes (must be larger than zero)
    void Free (void* p, size_t size);

    //! \brief Release ownership of the allocated memory
    //! \details Explicit type overload of opaque Free.
    //! \param [in] p Pointer to free
    template <typename T>
    void Free (T* p)
    {
        Free(reinterpret_cast<void*>(p), sizeof(T));
    }

    //! \brief Destruct and release ownership of the allocated memory
    //! \details Calls the destructor prior to freeing the memory, and should
    //!          only be used in conjunction with objects created via \ref New.
    //! \param [in] p Pointer to destruct and free
    template <typename T>
    void Delete (T* p)
    {
        p->~T();
        Free(reinterpret_cast<void*>(p), sizeof(T));
    }

    //! \brief Clear all heaps
    //! \details All block memory will be freed.
    //! \warning Free can no longer be called on individual allocs, therefore
    //!          it's highly recommended to not use RAII if using Clear.
    void Clear (void);

private:

    //! \struct block_node
    //! \brief Points to a block of heap allocated memory
    //! \details Also behaves as a forward linked list node for keeping track
    //!          of the available/consumed blocks.
    struct block_node
    {
        block_node* next = nullptr; //!< Next node in the forward list
    };

    //! \struct chunk
    //! \brief Chunk of heap-allocated memory
    //! \details The chunk is broken down into fixed-size blocks
    struct chunk
    {
        size_t block_size = 0;       //!< Size of the block components
        block_node* nodes = nullptr; //!< List of block nodes
    };

    chunk* m_chunks = nullptr;  //!< Chunk array
    size_t m_chunkCount = 0;    //!< Number of chunks consumed
    size_t m_chunkCapacity = 0; //!< Number of chunks allocated

    std::array<block_node*, NUM_BLOCK_SIZES> m_available; //!< Next available nodes

#ifdef RDGE_DEBUG
public:

    memory_profile mem_prof;

    struct usage_statistics
    {
        // stateful
        uint64 claimed = 0;  //!< Total memory claimed
        uint64 slack = 0;    //!< Total dead memory (block_size - claimed size)

        // aggregate
        size_t allocs[NUM_BLOCK_SIZES] = { }; //!< Number of allocs per block size
        size_t frees[NUM_BLOCK_SIZES] = { };  //!< Number of frees per block size
        size_t large_allocs = 0; //!< Number of allocs larger than supported size
    } usage;

    void PrintStats (std::ostream& os) const noexcept;
#endif
};

} // namespace rdge
