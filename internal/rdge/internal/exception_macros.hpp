#pragma once

#include <rdge/types.hpp>
#include <rdge/util/exception.hpp>

#ifndef RDGE_THROW
    #define RDGE_THROW(msg) \
        do { \
            throw RDGE::Exception(msg, __FILE__, __LINE__, __FUNCTION_NAME__); \
        } while (false)
#endif

#ifndef SDL_THROW
    #define SDL_THROW(msg, fn) \
        do { \
            throw RDGE::SDLException(msg, fn, __FILE__, __LINE__, __FUNCTION_NAME__); \
        } while (false)
#endif

#ifndef GL_THROW
    #define GL_THROW(msg, fn, code) \
        do { \
            throw RDGE::GLException(msg, fn, code, __FILE__, __LINE__, __FUNCTION_NAME__); \
        } while (false)
#endif
