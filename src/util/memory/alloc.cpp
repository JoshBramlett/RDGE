#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <cstdlib>
#include <errno.h>

// Checks for arithmetic overflow on the (size * num) calculation.
// Returns 1 if an overflow will occur.
// see https://github.com/gagern/gnulib/blob/master/lib/safe-alloc.c
#define SAFE_ALLOC_OVERSIZED(n, s) \
  ((size_t) (sizeof (ptrdiff_t) <= sizeof (size_t) ? -1 : -2) / (s) < (n))

namespace {

#ifdef RDGE_DEBUG
rdge::intrusive_list<rdge::memory_profile> s_profiles;
rdge::memory_profile s_anonymousProfile;
#endif

} // anonymous namespace

namespace rdge {

bool
safe_alloc (void** p, size_t size, size_t num, bool clear, memory_profile* profile)
{
    if (size == 0 || num == 0)
    {
        SDL_assert(false); // Valid case, remove assert if intentional
        *p = nullptr;
        return true;
    }

    if (UNLIKELY(SAFE_ALLOC_OVERSIZED(num, size)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG
    // TODO Size is prepended to the pointer to keep track of resident memory,
    //      however, it's of type size_t so it has an 8 byte alignment.
    //      Revisit when implementing SIMD

    size_t total_size = (num * size) + sizeof(size_t);
    void* poffset = malloc(total_size);
    if (UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    if (clear)
    {
        memset(poffset, 0, total_size);
    }

    memcpy(poffset, &total_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    if (!profile)
    {
        profile = &s_anonymousProfile;
    }

    profile->resident += total_size;
    profile->allocs++;
#else
    rdge::Unused(profile);

    *p = (clear) ? calloc(num, size) : malloc(num * size);
    if (UNLIKELY(*p == nullptr))
    {
        return false;
    }
#endif

    return true;
}

bool
safe_realloc (void** p, size_t size, size_t num, memory_profile* profile)
{
    if (size == 0 || num == 0)
    {
        SDL_assert(false); // Valid case, remove assert if intentional
        free(*p);
        *p = nullptr;
        return true;
    }

    if (UNLIKELY(SAFE_ALLOC_OVERSIZED(num, size)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG
    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t old_size = *(size_t*)actual_p;

    size_t new_size = (num * size) + sizeof(size_t);
    void* poffset = realloc(actual_p, new_size);
    if (UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    memcpy(poffset, &new_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    if (!profile)
    {
        profile = &s_anonymousProfile;
    }

    profile->resident += new_size - old_size;
    profile->reallocs++;
#else
    rdge::Unused(profile);

    void* tmp = realloc(*p, num * size);
    if (UNLIKELY(tmp == nullptr))
    {
        return false;
    }
    *p = tmp;
#endif

    return true;
}

#ifdef RDGE_DEBUG
void
debug_free (void** p, memory_profile* profile)
{
    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t size = *(size_t*)actual_p;

    if (!profile)
    {
        profile = &s_anonymousProfile;
    }

    profile->resident -= size;
    profile->frees++;

    free(actual_p);
    *p = nullptr;
}

void
register_memory_profile (memory_profile& profile)
{
    s_profiles.push_back(&profile);
}

void
unregister_memory_profile (memory_profile& profile)
{
    s_profiles.remove(&profile);
}
#endif

} // namespace rdge
