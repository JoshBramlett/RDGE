//! \headerfile <rdge/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/07/2016

#pragma once

#include <cstdint>
#include <string>

#ifdef RDGE_DEBUG
#include <iostream>
#endif

#define RDGE_VERSION "0.0.9"

/***********************************
/    Compiler specific defines
***********************************/

#ifndef UNLIKELY
    #if defined(COMPILER_GCC) || defined(__clang__)
        #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else
        #define UNLIKELY(x) (x)
    #endif
#endif

#if defined(COMPILER_GCC) || defined(__clang__)
    #define NOINLINE __attribute__((noinline))
#elif defined(COMPILER_MSVC)
    #define NOINLINE __declspec(noinline)
#else
    #define NOINLINE
#endif

#ifndef restrict
    #if defined(COMPILER_MSVC)
        #define restrict __declspec(restrict)
    #elif defined(COMPILER_GCC) || defined(__clang__)
        #define restrict __restrict__
    #endif
#endif

#ifndef __FUNCTION_NAME__
    #ifdef _MSC_VER
        #define __FUNCTION_NAME__   __FUNCTION__
    #else
        #define __FUNCTION_NAME__   __func__
    #endif
#endif

#ifndef __FILE_NAME__
    #define __FILE_NAME__ (strrchr(__FILE__, '/')                \
                              ? strrchr(__FILE__, '/') + 1       \
                              : strrchr(__FILE__, '\\')          \
                                  ? strrchr(__FILE__, '\\') + 1  \
                                  : __FILE__)
#endif

/***********************************
/           Byte Order
***********************************/

// The byte order is important for both SDL and OpenGL in regards
// to how it interprets data.  Specifically images.

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define BYTEORDER_R_MASK 0xff000000
    #define BYTEORDER_G_MASK 0x00ff0000
    #define BYTEORDER_B_MASK 0x0000ff00
    #define BYTEORDER_A_MASK 0x000000ff
#else
    #define BYTEORDER_R_MASK 0x000000ff
    #define BYTEORDER_G_MASK 0x0000ff00
    #define BYTEORDER_B_MASK 0x00ff0000
    #define BYTEORDER_A_MASK 0xff000000
#endif

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

/***********************************
/  Ignore unused variable warnings
***********************************/

template<class... T>
void Unused (T&&...) { }

/***********************************
/       OS Abstraction Types
***********************************/

using Int8  = std::int8_t;
using UInt8 = std::uint8_t;

using Int16  = std::int16_t;
using UInt16 = std::uint16_t;

using Int32  = std::int32_t;
using UInt32 = std::uint32_t;

using Int64  = std::int64_t;
using UInt64 = std::uint64_t;

#ifdef _WIN32
const char PathSeparator = '\\';
#else
const char PathSeparator = '/';
#endif

} // namespace RDGE

namespace rdge = RDGE;
