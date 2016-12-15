//! \headerfile <rdge/internal/hints.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/17/2016

#pragma once

#ifndef UNLIKELY
    #if defined(COMPILER_GCC) || defined(__clang__)
        #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else
        #define UNLIKELY(x) (x)
    #endif
#endif
