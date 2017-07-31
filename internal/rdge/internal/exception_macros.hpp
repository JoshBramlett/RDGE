//! \headerfile <rdge/internal/exception_macros.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>

#include <string>

#ifndef RDGE_THROW
    #define RDGE_THROW(msg) do { \
        ELOG() << "rdge::Exception!  what=" << msg; \
        throw rdge::Exception(msg, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif

#ifndef SDL_THROW
    #define SDL_THROW(msg, fn) do { \
        ELOG() << "rdge::SDLException!  what=" << msg << " fn=" << fn; \
        throw rdge::SDLException(msg, fn, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif

#ifndef GL_THROW
    #define GL_THROW(msg, fn, code) do { \
        ELOG() << "rdge::GLException!  what=" << msg << " fn=" << fn << " code=" << code; \
        throw rdge::GLException(msg, fn, code, __FILE__, __LINE__, FUNCTION_NAME); \
    } while (false)
#endif
