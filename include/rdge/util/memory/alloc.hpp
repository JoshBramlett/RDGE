//! \headerfile <rdge/util/memory/alloc.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/17/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

#define RDGE_MALLOC(ptr, size, profile) \
    rdge::safe_alloc((void**)&(ptr), size, 1, false, profile)

#define RDGE_MALLOC_N(ptr, num, profile) \
    rdge::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, false, profile)

#define RDGE_CALLOC(ptr, num, profile) \
    rdge::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, true, profile)

#define RDGE_REALLOC(ptr, num, profile) \
    rdge::safe_realloc((void**)&(ptr), sizeof(*(ptr)), num, profile)

#ifdef RDGE_DEBUG
#   define RDGE_FREE(ptr, profile) \
    rdge::debug_free((void**)&(ptr), profile)

#   define TRACK_MEMORY(profile) \
    rdge::register_memory_profile(profile)

#   define UNTRACK_MEMORY(profile) \
    rdge::unregister_memory_profile(profile)
#else
#   define RDGE_FREE(ptr, profile) do { free(ptr); ptr = nullptr; } while (0)
#   define TRACK_MEMORY(profile) do { } while(0)
#   define UNTRACK_MEMORY(profile) do { } while(0)
#endif

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

struct memory_profile : public intrusive_list_element<memory_profile>
{
    uint64 resident = 0; //!< System memory currently allocated (in bytes)
    size_t allocs = 0;   //!< Number of dynamic allocations
    size_t frees = 0;    //!< Number of frees
    size_t reallocs = 0; //!< Number of reallocations
};

bool
safe_alloc (void** p, size_t size, size_t num, bool clear, memory_profile* profile = nullptr);

bool
safe_realloc (void** p, size_t size, size_t num, memory_profile* profile = nullptr);

#ifdef RDGE_DEBUG
void
debug_free (void** p, memory_profile* profile = nullptr);

void
register_memory_profile (memory_profile& profile);

void
unregister_memory_profile (memory_profile& profile);
#endif

} // namespace rdge
