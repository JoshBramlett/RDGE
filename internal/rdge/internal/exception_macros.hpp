//! \headerfile <rdge/internal/exception_macros.hpp>
//! \author Josh Bramlett
//! \version 0.0.3
//! \date 05/13/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/internal/logger_macros.hpp>

#ifndef RDGE_THROW
    #define RDGE_THROW(msg) do { \
        ELOG("RDGE::Exception!  what=" + std::string(msg)); \
        throw RDGE::Exception(msg, __FILE__, __LINE__, __FUNCTION_NAME__); \
    } while (false)
#endif

#ifndef SDL_THROW
    #define SDL_THROW(msg, fn) do { \
        ELOG("RDGE::SDLException!  what=" + std::string(msg) + \
             " fn=" + std::string(fn)); \
        throw RDGE::SDLException(msg, fn, __FILE__, __LINE__, __FUNCTION_NAME__); \
    } while (false)
#endif

#ifndef GL_THROW
    #define GL_THROW(msg, fn, code) do { \
        ELOG("RDGE::GLException!  what=" + std::string(msg) + \
             " fn=" + std::string(fn) + " code=" + std::to_string(code)); \
        throw RDGE::GLException(msg, fn, code, __FILE__, __LINE__, __FUNCTION_NAME__); \
    } while (false)
#endif
