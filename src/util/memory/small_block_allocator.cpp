#include <rdge/util/memory/small_block_allocator.hpp>
#include <rdge/math/intrinsics.hpp>

#include <SDL_assert.h>

#include <cstdlib>
#include <memory> // call_once
#include <sstream>
#include <iomanip>
#include <limits>

namespace rdge {

namespace {

    // supported block sizes
    std::array<size_t, SmallBlockAllocator::NUM_BLOCK_SIZES> s_blockSizes {
        { 16, 32, 64, 96, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640 }
    };

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

    // allocate an array of empty heaps (chunks).  Each index will be allocated
    // on demand so on init the array contains pointers.
    m_chunks = (chunk*)calloc(CHUNK_ELEMENTS, sizeof(chunk));
    m_available.fill(nullptr);

#ifdef RDGE_DEBUG
    stats.allocs.fill(0);
    stats.frees.fill(0);
#endif
}

SmallBlockAllocator::~SmallBlockAllocator (void) noexcept
{
    for (size_t i = 0; i < m_chunkCount; ++i)
    {
        free(m_chunks[i].nodes);
    }

    free(m_chunks);
}

SmallBlockAllocator::SmallBlockAllocator (SmallBlockAllocator&& rhs) noexcept
    : m_chunkCapacity(rhs.m_chunkCapacity)
    , m_available(std::move(rhs.m_available)) // should be a copy anyway
{
    std::swap(m_chunks, rhs.m_chunks);
    std::swap(m_chunkCount, rhs.m_chunkCount);

#ifdef RDGE_DEBUG
    stats = std::move(rhs.stats);
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
        stats = std::move(rhs.stats);
#endif
    }

    return *this;
}

void*
SmallBlockAllocator::Alloc (size_t size)
{
    SDL_assert(size > 0);

    if (size > MAX_BLOCK_SIZE)
    {
#ifdef RDGE_DEBUG
        stats.large_allocs++;
#endif
        return malloc(size);
    }

    size_t index = s_blockSizeLookup[size];
    SDL_assert(0 <= index && index < NUM_BLOCK_SIZES);

    if (m_available[index])
    {
#ifdef RDGE_DEBUG
        stats.allocs[index]++;
        stats.in_use += s_blockSizes[index];
        stats.total_slack += s_blockSizes[index] - size;
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
        m_chunks = (chunk*)realloc(m_chunks, m_chunkCapacity * sizeof(chunk));
    }

    // No pre-allocated block is available - allocate a new heap
    chunk* c = m_chunks + m_chunkCount;
    c->nodes = (block_node*)malloc(CHUNK_SIZE);
    c->block_size = s_blockSizes[index];

    // Break up the heap into block sized partitions
    size_t last_index = (CHUNK_SIZE / c->block_size) - 1; // aka (block_count - 1)
    auto cursor = reinterpret_cast<uint8*>(c->nodes);
    for (size_t i = 1; i < last_index; ++i)
    {
        auto b = reinterpret_cast<block_node*>(cursor + (c->block_size * (i - 1)));
        b->next = reinterpret_cast<block_node*>(cursor + (c->block_size * i));

        SDL_assert((ptrdiff_t(b->next) - ptrdiff_t(b)) == ptrdiff_t(c->block_size));
    }

    auto last = reinterpret_cast<block_node*>(cursor + (c->block_size * last_index));
    last->next = nullptr;

    // Attach the new allocated blocks to available list
    m_available[index] = c->nodes->next;
    m_chunkCount++;

#ifdef RDGE_DEBUG
    stats.allocs[index]++;
    stats.in_use += c->block_size;
    stats.total_slack += c->block_size - size;
#endif

    return c->nodes;
}

void
SmallBlockAllocator::Free (void* p, size_t size)
{
    SDL_assert(p != nullptr && size > 0);

    if (size > MAX_BLOCK_SIZE)
    {
        free(p);
        return;
    }

    size_t index = s_blockSizeLookup[size];
    SDL_assert(0 <= index && index < NUM_BLOCK_SIZES);

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

    SDL_assert(found);

    stats.frees[index]++;
    stats.in_use -= block_size;
#endif

    block_node* node = static_cast<block_node*>(p);
    node->next = m_available[index];
    m_available[index] = node;
}

void
SmallBlockAllocator::Clear (void)
{
#ifdef RDGE_DEBUG
    stats.in_use = 0;
#endif

    for (size_t i = 0; i < m_chunkCount; ++i)
    {
        free(m_chunks[i].nodes);
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
        auto bigger = std::max(stats.allocs[i], stats.frees[i]);
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
             << std::setw(widths[i]) << std::left << stats.allocs[i] << " ";
        ss_f << " " << s_blockSizes[i] << ":"
             << std::setw(widths[i]) << std::left << stats.frees[i] << " ";
    }

    ss_a << " ]\n";
    ss_f << " ]\n";

    os << ss_a.str() << ss_f.str();
    os << "  total_slack:  " << stats.total_slack << " bytes\n"
       << "  large_allocs: " << stats.large_allocs << "\n"
       << "  in_use:       " << stats.in_use << " bytes\n";
}
#endif

} // namespace rdge
