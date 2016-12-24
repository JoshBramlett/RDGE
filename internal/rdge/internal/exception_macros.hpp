//! \headerfile <rdge/internal/exception_macros.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/internal/logger_macros.hpp>

#include <string>

#ifndef RDGE_THROW
    #define RDGE_THROW(msg) do { \
        ELOG("rdge::Exception!  what=" + std::string(msg)); \
        throw rdge::Exception(msg, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif

#ifndef SDL_THROW
    #define SDL_THROW(msg, fn) do { \
        ELOG("rdge::SDLException!  what=" + std::string(msg) + \
             " fn=" + std::string(fn)); \
        throw rdge::SDLException(msg, fn, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif

#ifndef GL_THROW
    #define GL_THROW(msg, fn, code) do { \
        ELOG("rdge::GLException!  what=" + std::string(msg) + \
             " fn=" + std::string(fn) + " code=" + std::to_string(code)); \
        throw rdge::GLException(msg, fn, code, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif
