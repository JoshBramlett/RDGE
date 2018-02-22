//! \headerfile <rdge/debug/memory.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/21/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/memory/alloc.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace debug {

#ifdef RDGE_DEBUG_MEMORY_TRACKER

//! \struct memory_bucket_data
//! \brief Collection of memory profiling data
struct memory_bucket_data
{
    std::string name;    //!< Bucket name
    uint64 resident = 0; //!< System memory currently allocated (in bytes)
    size_t allocs = 0;   //!< Number of dynamic allocations
    size_t frees = 0;    //!< Number of frees
    size_t reallocs = 0; //!< Number of reallocations
};

//! \var g_memoryBuckets
//! \brief Global list of tracked memory requests
extern memory_bucket_data g_memoryBuckets[memory_bucket_count];

#endif

} // namespace debug
} // namespace rdge
