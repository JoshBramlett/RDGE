//! \headerfile <rdge/util/memory/alloc.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 08/17/2017

#pragma once

#include <rdge/core.hpp>

// - Alignment
// To track resident memory, the size of the allocation is intrusively prepended to the
// request.  The problem is it's offset by sizeof(size_t), which generally has an 8
// byte alignment.
// TODO SIMD requires 16 byte alignment.

    //! \def RDGE_MALLOC(size, bucket)
    //! \brief Tracked cstdlib style dynamic allocation (by size)
#   define RDGE_MALLOC(size, bucket) \
    rdge::detail::safe_alloc(size, bucket)

    //! \def RDGE_REALLOC(ptr, size, bucket)
    //! \brief Tracked cstdlib style dynamic reallocation (by size)
#   define RDGE_REALLOC(ptr, size, bucket) \
    rdge::detail::safe_realloc((void**)&(ptr), size, bucket)

    //! \def RDGE_TMALLOC(ptr, num, bucket)
    //! \brief Tracked type deduced dynamic allocation (by count)
#   define RDGE_TMALLOC(ptr, num, bucket) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, false, bucket)

    //! \def RDGE_TCALLOC(ptr, num, bucket)
    //! \brief Tracked type deduced zero initialized dynamic allocation (by count)
#   define RDGE_TCALLOC(ptr, num, bucket) \
    rdge::detail::safe_alloc((void**)&(ptr), sizeof(*(ptr)), num, true, bucket)

    //! \def RDGE_TREALLOC(ptr, num, bucket)
    //! \brief Tracked type deduced dynamic reallocation (by count)
#   define RDGE_TREALLOC(ptr, num, bucket) \
    rdge::detail::safe_realloc((void**)&(ptr), sizeof(*(ptr)), num, bucket)

    //! \def RDGE_FREE(ptr, bucket)
    //! \brief Free memory allocated by macros
#   define RDGE_FREE(ptr, bucket) \
    rdge::detail::debug_free((void**)&(ptr), bucket)

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum memory_bucket
//! \brief Categories for tracking memory allocations
enum memory_bucket
{
    memory_bucket_none = 0,
    memory_bucket_ext,
    memory_bucket_debug,
    memory_bucket_assets,
    memory_bucket_graphics,
    memory_bucket_physics,
    memory_bucket_allocators,
    memory_bucket_containers,

    // keep last
    memory_bucket_count
};

namespace detail {

//!@{ Use macros - do not call directly
void* safe_alloc (size_t, memory_bucket);
bool safe_alloc (void**, size_t, size_t, bool, memory_bucket);
void* safe_realloc (void**, size_t, memory_bucket);
bool safe_realloc (void**, size_t, size_t, memory_bucket);
void debug_free (void**, memory_bucket);
//!@}

} // namespace detail

//! \brief memory_bucket stream output operator
std::ostream& operator<< (std::ostream&, memory_bucket);

//!@{ memory_bucket string conversions
std::string to_string (memory_bucket);
//!@}

} // namespace rdge
