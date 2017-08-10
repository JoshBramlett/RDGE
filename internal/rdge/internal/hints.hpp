//! \headerfile <rdge/internal/hints.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/17/2016

#pragma once

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define UNLIKELY(x) (x)
#endif
