#include <rdge/util/memory/small_block_allocator.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/debug/assert.hpp>

#include <cstdlib>
#include <memory> // call_once
#include <sstream>
#include <iomanip>
#include <limits>

namespace rdge {

namespace {

// supported block sizes
std::array<size_t, SmallBlockAllocator::NUM_BLOCK_SIZES> s_blockSizes { {
    16,  32,  64,  96, 128,
    160, 192, 224, 256, 320,
    384, 448, 512, SmallBlockAllocator::MAX_BLOCK_SIZE
} };

// look up table to get the block size required from the requested size
std::once_flag s_populateLookup;
std::array<uint8, SmallBlockAllocator::MAX_BLOCK_SIZE + 1> s_blockSizeLookup;

} // anonymous namespace

SmallBlockAllocator::SmallBlockAllocator (void)
    : m_chunkCapacity(CHUNK_ELEMENTS)
{
    std::call_once(s_populateLookup, []() {
        for (size_t i = 1, j = 0; i <= MAX_BLOCK_SIZE; ++i)
        {
            if (i > s_blockSizes[j])
            {
                j++;
            }

            s_blockSizeLookup[i] = static_cast<uint8>(j);
        }
    });

    m_available.fill(nullptr);

    // Allocate the list used to point to chunks.  Each chunk will be allocated
    // on demand and assigned to this list.
    if (RDGE_UNLIKELY(!RDGE_TCALLOC(m_chunks, CHUNK_ELEMENTS, memory_bucket_allocators)))
    {
        RDGE_THROW("Memory allocation failed");
    }
}

SmallBlockAllocator::~SmallBlockAllocator (void) noexcept
{
    for (size_t i = 0; i < m_chunkCount; ++i)
    {
        RDGE_FREE(m_chunks[i].nodes, memory_bucket_allocators);
    }

    RDGE_FREE(m_chunks, memory_bucket_allocators);
}

SmallBlockAllocator::SmallBlockAllocator (SmallBlockAllocator&& rhs) noexcept
    : m_chunkCapacity(rhs.m_chunkCapacity)
    , m_available(std::move(rhs.m_available)) // should be a copy anyway
{
    std::swap(m_chunks, rhs.m_chunks);
    std::swap(m_chunkCount, rhs.m_chunkCount);

#ifdef RDGE_DEBUG
    usage = std::move(rhs.usage);
#endif
}

SmallBlockAllocator&
SmallBlockAllocator::operator= (SmallBlockAllocator&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_chunks, rhs.m_chunks);
        std::swap(m_chunkCount, rhs.m_chunkCount);

        m_chunkCapacity = rhs.m_chunkCapacity;
        m_available = std::move(rhs.m_available);

#ifdef RDGE_DEBUG
        usage = std::move(rhs.usage);
#endif
    }

    return *this;
}

void*
SmallBlockAllocator::Alloc (size_t size)
{
    RDGE_ASSERT(size > 0);

    if (size > MAX_BLOCK_SIZE)
    {
#ifdef RDGE_DEBUG
        usage.large_allocs++;
#endif
        void* result = RDGE_MALLOC(size, memory_bucket_allocators);
        if (RDGE_UNLIKELY(!result))
        {
            RDGE_THROW("Memory allocation failed");
        }

        return result;
    }

    size_t index = s_blockSizeLookup[size];
    RDGE_ASSERT(0 <= index && index < NUM_BLOCK_SIZES);

    if (m_available[index])
    {
#ifdef RDGE_DEBUG
        usage.allocs[index]++;
        usage.claimed += size;
        usage.slack += s_blockSizes[index] - size;
#endif
        // use already allocated block if available
        block_node* node = m_available[index];
        m_available[index] = node->next;
        return node;
    }

    if (m_chunkCount == m_chunkCapacity)
    {
        // The number of heaps is exhausted - reallocate
        m_chunkCapacity += CHUNK_ELEMENTS;
        if (RDGE_UNLIKELY(!RDGE_TREALLOC(m_chunks, m_chunkCapacity, memory_bucket_allocators)))
        {
            RDGE_THROW("Memory allocation failed");
        }
    }

    // No pre-allocated block is available - allocate a new heap
    chunk* c = m_chunks + m_chunkCount;
    c->block_size = s_blockSizes[index];
    c->nodes = (block_node*)RDGE_MALLOC(CHUNK_SIZE, memory_bucket_allocators);
    if (RDGE_UNLIKELY(!c->nodes))
    {
        RDGE_THROW("Memory allocation failed");
    }

    // Break up the heap into block sized partitions
    size_t last_index = (CHUNK_SIZE / c->block_size) - 1; // aka (block_count - 1)
    auto cursor = reinterpret_cast<uint8*>(c->nodes);
    for (size_t i = 0; i < last_index; ++i)
    {
        auto b = reinterpret_cast<block_node*>(cursor + (c->block_size * i));
        b->next = reinterpret_cast<block_node*>(cursor + (c->block_size * (i + 1)));

        RDGE_ASSERT((ptrdiff_t(b->next) - ptrdiff_t(b)) == ptrdiff_t(c->block_size));
    }

    auto last = reinterpret_cast<block_node*>(cursor + (c->block_size * last_index));
    last->next = nullptr;

    // Attach the new allocated blocks to available list
    m_available[index] = c->nodes->next;
    m_chunkCount++;

#ifdef RDGE_DEBUG
    usage.allocs[index]++;
    usage.claimed += size;
    usage.slack += c->block_size - size;
#endif

    return c->nodes;
}

void
SmallBlockAllocator::Free (void* p, size_t size)
{
    RDGE_ASSERT(p != nullptr && size > 0);

    if (size > MAX_BLOCK_SIZE)
    {
        RDGE_FREE(p, memory_bucket_allocators);
        return;
    }

    size_t index = s_blockSizeLookup[size];
    RDGE_ASSERT(0 <= index && index < NUM_BLOCK_SIZES);

#ifdef RDGE_DEBUG
    size_t block_size = s_blockSizes[index];
    bool found = false;
    for (size_t i = 0; i < m_chunkCount; ++i)
    {
        chunk* c = m_chunks + i;
        if (c->block_size == block_size)
        {
            if ((uint8*)c->nodes <= (uint8*)p &&
                (uint8*)p + c->block_size <= (uint8*)c->nodes + CHUNK_SIZE)
            {
                found = true;
                break;
            }
        }
    }

    RDGE_ASSERT(found);

    usage.frees[index]++;
    usage.claimed -= size;
    usage.slack -= block_size - size;
#endif

    block_node* node = static_cast<block_node*>(p);
    node->next = m_available[index];
    m_available[index] = node;
}

void
SmallBlockAllocator::Clear (void)
{
#ifdef RDGE_DEBUG
    usage.claimed = 0;
    usage.slack = 0;
#endif

    for (size_t i = 0; i < m_chunkCount; ++i)
    {
        RDGE_FREE(m_chunks[i].nodes, memory_bucket_allocators);
    }

    m_chunkCount = 0;
    memset(m_chunks, 0, m_chunkCapacity * sizeof(chunk));
    m_available.fill(nullptr);
}

#ifdef RDGE_DEBUG
void
SmallBlockAllocator::PrintStats (std::ostream& os) const noexcept
{
    os << "[SmallBlockAllocator]\n";

    // align to largest value
    std::array<uint32, NUM_BLOCK_SIZES> widths;
    for (size_t i = 0; i < NUM_BLOCK_SIZES; ++i)
    {
        auto bigger = std::max(usage.allocs[i], usage.frees[i]);
        widths[i] = (bigger == 0) ? 1
                  : (bigger >= std::numeric_limits<int>::max()) ? 10
                  : static_cast<uint32>(std::log10(static_cast<double>(bigger)) + 1);
    }

    std::ostringstream ss_a;
    std::ostringstream ss_f;
    ss_a << "  allocs [ ";
    ss_f << "  frees  [ ";

    for (size_t i = 0; i < NUM_BLOCK_SIZES; i++)
    {
        ss_a << " " << s_blockSizes[i] << ":"
             << std::setw(widths[i]) << std::left << usage.allocs[i] << " ";
        ss_f << " " << s_blockSizes[i] << ":"
             << std::setw(widths[i]) << std::left << usage.frees[i] << " ";
    }

    ss_a << " ]\n";
    ss_f << " ]\n";

    os << ss_a.str() << ss_f.str();
    os << "  large_allocs: " << usage.large_allocs << "\n"
       << "  claimed:      " << usage.claimed << " bytes\n"
       << "  slack:        " << usage.slack << " bytes\n";
}
#endif

} // namespace rdge
