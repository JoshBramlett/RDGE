//! \headerfile <rdge/util/memory/small_block_allocator.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/27/2017

#pragma once

#include <rdge/core.hpp>

#include <array>
#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

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

//! \class SmallBlockAllocator
//! \brief Dynamically allocates blocks of memory for small objects
//! \details Provides heap allocation for smaller objects.  An object is
//!          assigned a slot in an pre-allocated block, therefore like
//!          types have the added benefit of cache locality.  If an
//!          allocation request is larger than the maximum supported
//!          it will default to using malloc.  Inspired by b2BlockAllocator
//!          in Box2D.
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

    chunk* m_chunks = nullptr;  //!< Chunk array
    size_t m_chunkCount = 0;    //!< Number of chunks consumed
    size_t m_chunkCapacity = 0; //!< Number of chunks allocated

    std::array<block_node*, NUM_BLOCK_SIZES> m_available; //!< Next available nodes

#ifdef RDGE_DEBUG
public:

    struct statistics
    {
        // aggregate usage
        std::array<uint64, NUM_BLOCK_SIZES> allocs; //!< Number of allocs per size
        std::array<uint64, NUM_BLOCK_SIZES> frees;  //!< Number of frees per size
        uint64 total_slack = 0;  //!< Total unused space (block_size - request size)
        uint64 large_allocs = 0; //!< Number of allocs larger than supported size

        // stateful
        uint64 in_use = 0; //!< Total memory claimed
    } stats;

    void PrintStats (std::ostream& os) const noexcept;
#endif

};

} // namespace rdge
