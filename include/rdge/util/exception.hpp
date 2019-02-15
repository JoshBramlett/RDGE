//! \headerfile <rdge/util/exception.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 10/25/2015

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/logger.hpp>

#include <string>
#include <exception>

#define RDGE_THROW(msg) do {                                       \
    ELOG() << "rdge::Exception!  what=" << msg;                    \
    throw rdge::Exception(msg, __FILE__, __LINE__, FUNCTION_NAME); \
} while (false)

#define SDL_THROW(msg, fn) do {                                           \
    ELOG() << "rdge::SDLException!  what=" << msg << " fn=" << fn;        \
    throw rdge::SDLException(msg, fn, __FILE__, __LINE__, FUNCTION_NAME); \
} while (false)

#define GL_THROW(msg, fn, code) do {                                                  \
    ELOG() << "rdge::GLException!  what=" << msg << " fn=" << fn << " code=" << code; \
    throw rdge::GLException(msg, fn, code, __FILE__, __LINE__, FUNCTION_NAME);        \
} while (false)

#define RDGE_THROW_ALLOC_FAILED() RDGE_THROW("Memory allocation failed")

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Exception
//! \brief Generic runtime error exception
//! \details Can optionally contain information as to the location of
//!          the exception.
class Exception : public std::runtime_error
{
public:
    //! \brief Exception ctor
    //! \param [in] message Exception details
    //! \param [in] file File where exception occurred
    //! \param [in] line Line where exception occurred
    //! \param [in] fn_name Function name where exception occurred
    explicit Exception (const std::string& message,
                        std::string        file    = "",
                        uint32             line    = 0,
                        std::string        fn_name = "");

    //! \brief Exception dtor
    virtual ~Exception (void) noexcept = default;

    //!@{ Copy and move enabled
    Exception (const Exception&) = default;
    Exception& operator= (const Exception&) = default;
    Exception (Exception&&) noexcept = default;
    Exception& operator= (Exception&&) noexcept = default;
    //!@}

    //! \brief File where the exception was thrown
    virtual const std::string& File (void) const final { return m_file; }

    //! \brief Line number where the exception was thrown
    virtual uint32 Line (void) const final { return m_line; }

    //! \brief Function where the exception was thrown
    virtual const std::string& Function (void) const final { return m_function; }

    //! \brief Name of the file where the exception was thrown
    //! \details Full path will be returned if parsing fails
    //! \return File name
    virtual std::string FileName (void) const final;

private:
    std::string  m_file;
    rdge::uint32 m_line;
    std::string  m_function;
};

//! \class SDLException
//! \brief Wraps SDL error information
//! \details Should be thrown any time the SDL C style error checking
//!          fails.  Contains data pertinent to the failure, including
//!          the failed function name and the generated SDL_GetError()
//!          string.  This includes error checking performed on all SDL
//!          extension libraries.
//! \note SDL extension libraries have their own "GetError" functions, but
//!       all set the stardard value available through SDL_GetError() so
//!       there's no need to abstract further
class SDLException final : public Exception
{
public:
    //! \brief SDLException ctor
    //! \param [in] message Exception details
    //! \param [in] sdl_fn_name SDL function which failed
    //! \param [in] file File where exception occurred
    //! \param [in] line Line where exception occurred
    //! \param [in] parent_fn_name Function name where exception occurred
    explicit SDLException (const std::string& message,
                           std::string        sdl_fn_name,
                           std::string        file           = "",
                           uint32             line           = 0,
                           std::string        parent_fn_name = "");

    //! \brief SDLException dtor
    ~SDLException (void) noexcept = default;

    //! \brief Get the SDL function name which failed
    const std::string& SDLFunction (void) const { return m_SDLFunction; }

    //! \brief Get the SDL generated error message
    const std::string& SDLError (void) const { return m_SDLError; }

private:
    std::string m_SDLFunction;
    std::string m_SDLError;
};

//! \class GLException
//! \brief Wraps OpenGL error information
//! \details Should be thrown any time an OpenGL call or status check failed.
//!          If the error code is zero, the exception was created from a
//!          failed status check.
class GLException final : public Exception
{
public:
    //! \brief GLException ctor
    //! \param [in] message Exception details
    //! \param [in] gl_fn_name OpenGL function which failed
    //! \param [in] gl_error_code OpenGL defined error code
    //! \param [in] file File where exception occurred
    //! \param [in] line Line where exception occurred
    //! \param [in] parent_fn_name Function name where exception occurred
    explicit GLException (const std::string& message,
                          std::string        gl_fn_name,
                          uint32             gl_error_code  = 0,
                          std::string        file           = "",
                          uint32             line           = 0,
                          std::string        parent_fn_name = "");

    //! \brief GLException dtor
    ~GLException (void) noexcept = default;

    //! \brief Get the OpenGL function name which failed
    const std::string& GLFunction (void) const { return m_GLFunction; }

    //! \brief Get the OpenGL error code
    uint32 GLErrorCode (void) const { return m_GLErrorCode; }

    //! \brief Get the OpenGL error code in string format
    std::string GLErrorCodeString (void) const;

private:
    std::string m_GLFunction;
    uint32      m_GLErrorCode;
};

} // namespace rdge
