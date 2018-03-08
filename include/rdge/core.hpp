//! \headerfile <rdge/core.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/17/2016

#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <ostream>

#ifdef RDGE_DEBUG
#include <iostream>
#include <iomanip>
#endif

#define RDGE_VERSION "0.0.11"

/***********************************
/        Debugging defines
***********************************/

#define RDGE_DEBUG_MEMORY_TRACKER

/***********************************
/    Compiler specific defines
***********************************/

#ifndef FUNCTION_NAME
    #ifdef _MSC_VER
        #define FUNCTION_NAME __FUNCTION__
    #else
        #define FUNCTION_NAME __func__
    #endif
#endif

#ifndef FILE_NAME
    #define FILE_NAME (strrchr(__FILE__, '/')                \
                          ? strrchr(__FILE__, '/') + 1       \
                          : strrchr(__FILE__, '\\')          \
                              ? strrchr(__FILE__, '\\') + 1  \
                              : __FILE__)
#endif

/***********************************
/              Misc
***********************************/

#define SET_FLAG(_cond, _var, _flag) if (_cond) { _var |= _flag; } else { _var &= ~_flag; }

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

/***********************************
/  Ignore unused variable warnings
***********************************/

// Ignore unused variable warnings
template<class... T>
void Unused (T&&...) { }

/***********************************
/       OS Abstraction Types
***********************************/

using int8   = std::int8_t;
using uint8  = std::uint8_t;
using int16  = std::int16_t;
using uint16 = std::uint16_t;
using int32  = std::int32_t;
using uint32 = std::uint32_t;
using int64  = std::int64_t;
using uint64 = std::uint64_t;

// TODO Make sure float is 4 bytes.  Is there a cross platform type?
using float32 = float;
using float64 = double;

constexpr float32 qnan32 = std::numeric_limits<float32>::quiet_NaN();
constexpr float64 qnan64 = std::numeric_limits<float64>::quiet_NaN();

} // namespace rdge
