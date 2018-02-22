#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

#include <cstdlib> // malloc, realloc, free
#include <cstring> // strrchr
#include <sstream>
#include <errno.h>

#ifdef RDGE_DEBUG_MEMORY_TRACKER
#include <rdge/debug/memory.hpp>
#endif

// Checks for arithmetic overflow on the (size * num) calculation.
// Returns 1 if an overflow will occur.
// see https://github.com/gagern/gnulib/blob/master/lib/safe-alloc.c
#define SAFE_ALLOC_OVERSIZED(n, s) \
  ((size_t) (sizeof (ptrdiff_t) <= sizeof (size_t) ? -1 : -2) / (s) < (n))

namespace rdge {
namespace detail {

// The versions of safe_alloc and safe_realloc that return a pointer are
// to provide a c-style interface for use when overriding allocations in
// external libraries (namely stb)

void*
safe_alloc (size_t sz, memory_bucket id)
{
    void* p = nullptr;
    safe_alloc((void**)&(p), sz, 1, false, id);

    return p;
}

bool
safe_alloc (void** p, size_t sz, size_t num, bool clear, memory_bucket id)
{
    if (sz == 0 || num == 0)
    {
        *p = nullptr;
        return true;
    }

    if (RDGE_UNLIKELY(SAFE_ALLOC_OVERSIZED(num, sz)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    size_t total_size = (num * sz) + sizeof(size_t);
    void* poffset = malloc(total_size);
    if (RDGE_UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    if (clear)
    {
        memset(poffset, 0, total_size);
    }

    memcpy(poffset, &total_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    auto& bucket = debug::g_memoryBuckets[id];
    bucket.resident += total_size;
    bucket.allocs++;
#else
    rdge::Unused(id);

    *p = (clear) ? calloc(num, sz) : malloc(num * sz);
    if (RDGE_UNLIKELY(*p == nullptr))
    {
        return false;
    }
#endif

    return true;
}

void*
safe_realloc (void** p, size_t sz, memory_bucket id)
{
    safe_realloc(p, sz, 1, id);
    return *p;
}

bool
safe_realloc (void** p, size_t sz, size_t num, memory_bucket id)
{
    if (sz == 0 || num == 0)
    {
        debug_free(p, id);
        return true;
    }

    if (RDGE_UNLIKELY(SAFE_ALLOC_OVERSIZED(num, sz)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    if (*p == nullptr)
    {
        return safe_alloc(p, sz, num, false, id);
    }

    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t old_size = *(size_t*)actual_p;

    size_t new_size = (num * sz) + sizeof(size_t);
    void* poffset = realloc(actual_p, new_size);
    if (RDGE_UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    memcpy(poffset, &new_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    auto& bucket = debug::g_memoryBuckets[id];
    bucket.resident += new_size - old_size;
    bucket.reallocs++;
#else
    rdge::Unused(id);

    void* tmp = realloc(*p, num * sz);
    if (RDGE_UNLIKELY(tmp == nullptr))
    {
        return false;
    }
    *p = tmp;
#endif

    return true;
}

void
debug_free (void** p, memory_bucket id)
{
    if (*p == nullptr)
    {
        return;
    }

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t sz = *(size_t*)actual_p;

    auto& bucket = debug::g_memoryBuckets[id];
    bucket.resident -= sz;
    bucket.frees++;

    free(actual_p);
    *p = nullptr;
#else
    rdge::Unused(id);

    free(*p);
    *p = nullptr;
#endif
}

} // namespace detail

std::ostream&
operator<< (std::ostream& os, memory_bucket value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (memory_bucket value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, '_') + 1); break;
        CASE(memory_bucket_none)
        CASE(memory_bucket_ext)
        CASE(memory_bucket_debug)
        CASE(memory_bucket_assets)
        CASE(memory_bucket_graphics)
        CASE(memory_bucket_physics)
        CASE(memory_bucket_allocators)
        CASE(memory_bucket_containers)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "unknown[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

} // namespace rdge
