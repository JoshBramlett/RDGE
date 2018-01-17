//! \headerfile <rdge/util/compiler.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/17/2018

#pragma once

#if __has_builtin(__builtin_expect)
    #define RDGE_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define RDGE_LIKELY(x) __builtin_expect(!!(x), 1)
#else
    #define RDGE_UNLIKELY(x) (x)
    #define RDGE_LIKELY(x) (x)
#endif

#if __has_attribute(always_inline)
    #define RDGE_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define RDGE_ALWAYS_INLINE __forceinline
#else
    #define RDGE_ALWAYS_INLINE
#endif
