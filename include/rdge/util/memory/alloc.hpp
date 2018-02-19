//! \headerfile <rdge/util/memory/alloc.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/17/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

// (notes)
//
// - Alignment
// To track resident memory, the size of the allocation is intrusively prepended to the
// request.  The problem is it's offset by sizeof(size_t), which generally has an 8
// byte alignment.
// TODO SIMD requires 16 byte alignment.
//
// - Categories
// Parts of a categorizing system are in place.  Specifically the memory_profile_subsystem
// enum and the array of lists that track memory.  The problem I was running into when
// attempting to categorize was if the profile was not set to explicitly track it was
// lost, and decided it wasn't worth the risk of missing reported allocations to have
// them categorized.
// Also, categorizing itself isn't that easy because the classes that allocate memory
// (the containers) are generic and may be several levels deep in the callstack from
// where I would set the category.
// TODO Find an elegant way to track memory
// TODO Improve ImGui reporting window
//
// - Macros
// Macros are all tied to a memory_profile object.  Original implementation was with
// container classes, but depending on further use this tie may need to be severed.

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    //! \def RDGE_MALLOC_SZ(size, profile)
    //! \brief Dynamic allocation by size
#   define RDGE_MALLOC_SZ(size, profile) \
    rdge::detail::safe_alloc(size, profile)

    //! \def RDGE_REALLOC_SZ(size, profile)
    //! \brief Dynamic reallocation by size
#   define RDGE_REALLOC_SZ(ptr, size, profile) \
    rdge::detail::safe_realloc((void**)&(ptr), size, profile)

    //! \def RDGE_MALLOC(ptr, size, profile)
    //! \brief Dynamic allocation by size
#   define RDGE_MALLOC(ptr, size, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), size, 1, false, profile)

    //! \def RDGE_MALLOC_N(ptr, num, profile)
    //! \brief Dynamic allocation of num elements (type deduced by pointer)
#   define RDGE_MALLOC_N(ptr, num, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, false, profile)

    //! \def RDGE_CALLOC(ptr, num, profile)
    //! \brief Zero initialized dynamic allocation of num elements (type deduced by pointer)
#   define RDGE_CALLOC(ptr, num, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, true, profile)

    //! \def RDGE_REALLOC(ptr, num, profile)
    //! \brief Dynamic reallocation of num elements (type deduced by pointer)
#   define RDGE_REALLOC(ptr, num, profile) \
    rdge::detail::safe_realloc((void**)&(ptr), sizeof(*(ptr)), num, profile)

#   define RDGE_FREE(ptr, profile) \
    rdge::detail::debug_free((void**)&(ptr), profile)

//!@{
//! The following are used within a class to simplify adding a memory tracker.
//! Generally used with containers that dynamically allocate memory.

//! \def MEMORY_PROFILE_MEMBER
//! \brief Define memory_profile member variable (must be public)
#define MEMORY_PROFILE_MEMBER rdge::detail::memory_profile mem_prof;

//! \def TRACK_MEMORY_PROFILE
//! \brief Registers the memory_profile with the tracker
//! \details Place in the constructor before any allocations are made.
#define TRACK_MEMORY_PROFILE() \
    rdge::detail::register_memory_profile(mem_prof, FUNCTION_NAME)

//! \def UNTRACK_MEMORY_PROFILE
//! \brief Unregisters the memory_profile with the tracker
//! \details Place in the destructor after any frees are made
#define UNTRACK_MEMORY_PROFILE() \
    rdge::detail::unregister_memory_profile(mem_prof)

//! \def SWAP_MEMORY_PROFILE
//! \brief Swap memory_profile members (used in move semantics)
#define SWAP_MEMORY_PROFILE() std::swap(mem_prof, rhs.mem_prof)
//!@}

#else
#   define RDGE_MALLOC_SZ(size, profile) \
    rdge::detail::safe_alloc(size, nullptr)
#   define RDGE_REALLOC_SZ(ptr, size, profile) \
    rdge::detail::safe_realloc((void**)&(ptr), size, nullptr)
#   define RDGE_MALLOC(ptr, size, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), size, 1, false, nullptr)
#   define RDGE_MALLOC_N(ptr, num, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, false, nullptr)
#   define RDGE_CALLOC(ptr, num, profile) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, true, nullptr)
#   define RDGE_REALLOC(ptr, num, profile) \
    rdge::detail::safe_realloc((void**)&(ptr), sizeof(*(ptr)), num, nullptr)
#   define RDGE_FREE(ptr, profile) do { free(ptr); ptr = nullptr; } while (0)

#   define MEMORY_PROFILE_MEMBER
#   define TRACK_MEMORY_PROFILE() do { } while(0)
#   define UNTRACK_MEMORY_PROFILE() do { } while(0)
#   define SWAP_MEMORY_PROFILE() do { } while(0)
#endif

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace detail {

//! \struct memory_profile
//! \brief Collection of memory profiling data
struct memory_profile : public intrusive_list_element<memory_profile>
{
    uint64 resident = 0;        //!< System memory currently allocated (in bytes)
    const char* name = nullptr; //!< Display name
    size_t allocs = 0;          //!< Number of dynamic allocations
    size_t frees = 0;           //!< Number of frees
    size_t reallocs = 0;        //!< Number of reallocations
};

//! \enum memory_profile_subsystem
//! \brief Categories for organizing profiles
enum memory_profile_subsystem
{
    memory_profile_subsystem_none = 0,
    memory_profile_subsystem_physics,
    memory_profile_subsystem_graphics,
    memory_profile_subsystem_assets,

    memory_profile_lib_stb,

    memory_profile_subsystem_count
};

//! \brief Use macros - do not call directly
void* safe_alloc (size_t, memory_profile* = nullptr);

//! \brief Use macros - do not call directly
void* safe_realloc (void**, size_t, memory_profile* = nullptr);

//! \brief Use macros - do not call directly
bool safe_alloc (void**, size_t, size_t, bool, memory_profile* = nullptr);

//! \brief Use macros - do not call directly
bool safe_realloc (void**, size_t, size_t, memory_profile* = nullptr);

#ifdef RDGE_DEBUG_MEMORY_TRACKER
void debug_free (void**, memory_profile* = nullptr);
void register_memory_profile (memory_profile&, const char*);
void unregister_memory_profile (memory_profile&);
#endif

} // namespace detail

#ifdef RDGE_DEBUG
namespace debug {

// TODO MEMORY_TRACKING_ENABLED is only for ImGui rendering.  Could be moved
//      to something like ImGui settings in the future
#ifdef RDGE_DEBUG_MEMORY_TRACKER
static constexpr bool MEMORY_TRACKING_ENABLED = true;
#else
static constexpr bool MEMORY_TRACKING_ENABLED = false;
#endif

//! \brief Display ImGui memory tracker window
void ShowMemoryTracker (bool* = nullptr);

} // namespace debug
#endif

} // namespace rdge
