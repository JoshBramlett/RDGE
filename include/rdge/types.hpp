//! \headerfile <rdge/types.hpp>
//! \author Josh Bramlett
//! \version 0.0.5
//! \date 05/17/2016

#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <stdint.h>
#include <memory>
#include <string>

#include <iostream>

#define RDGE_VERSION "0.0.5"

// TODO: This needs to be a configuration flag
#define RDGE_DEBUG

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

typedef signed   char Int8;
typedef unsigned char UInt8;

typedef signed   short Int16;
typedef unsigned short UInt16;

typedef signed   int Int32;
typedef unsigned int UInt32;

#ifdef _MSC_VER
typedef signed   __int64 Int64;
typedef unsigned __int64 UInt64;
#else
typedef int64_t  Int64;
typedef uint64_t UInt64;
#endif

#ifdef _WIN32
const char PathSeparator = '\\';
#else
const char PathSeparator = '/';
#endif

} // namespace RDGE
