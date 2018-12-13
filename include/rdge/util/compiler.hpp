//! \headerfile <rdge/util/compiler.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/17/2018

#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#include <sal.h>
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#ifndef __has_extension
# define __has_extension(x) 0
#endif

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#ifndef __has_cpp_attribute
# define __has_cpp_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

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

#if __has_attribute(noinline)
    #define RDGE_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define RDGE_NO_INLINE __declspec(noinline)
#else
    #define RDGE_NO_INLINE
#endif

#if __has_builtin(__builtin_ffsll)
    #define RDGE_LSB(x) __builtin_ffsll(x)
#else
    #pragma intrinsic(_BitScanForward)
    inline rdge::int32 RDGE_LSB(long long x)
    {
		unsigned long index = 0;
		if (_BitScanForward(&index, static_cast<unsigned long>(x)))
		{
			return static_cast<rdge::int32>(index);
		}

		return 0;
	}
#endif

#if __has_builtin(__builtin_fpclassify)
    #define RDGE_FPCLASSIFY(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#else
    #define RDGE_FPCLASSIFY(x) std::fpclassify(x)
#endif

#ifndef restrict
    #if defined(COMPILER_GCC) || defined(__clang__)
        #define restrict __restrict__
    #elif defined(COMPILER_MSVC)
        #define restrict __declspec(restrict)
    #else
        #define restrict
    #endif
#endif

