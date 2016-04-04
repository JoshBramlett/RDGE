//! \headerfile <rdge/util/exception.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 10/25/2015
//! \bug

#pragma once

#include <rdge/types.hpp>

#include <string>
#include <exception>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Util {

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
    explicit Exception (
                        const std::string& message,
                        std::string        file     = "",
                        RDGE::UInt32       line     = 0,
                        std::string        fn_name  = ""
                       );

    //! \brief Exception dtor
    virtual ~Exception (void) { }

    //! \brief Exception Copy ctor
    //! \details Default-copyable
    Exception (const Exception&) = default;

    //! \brief Exception Move ctor
    //! \details Default-moveable
    Exception (Exception&&) = default;

    //! \brief Exception Copy Assignment Operator
    //! \details Default-copyable
    Exception& operator= (const Exception&) = default;

    //! \brief Exception Move Assignment Operator
    //! \details Default-moveable
    Exception& operator= (Exception&&) = default;

    //! \brief File where the exception was thrown
    //! \return Full path of the file
    virtual std::string File (void) const final
    {
        return m_file;
    }

    //! \brief Line number where the exception was thrown
    //! \return Line number
    virtual RDGE::UInt32 Line (void) const final
    {
        return m_line;
    }

    //! \brief Function where the exception was thrown
    //! \return Function name
    virtual std::string Function (void) const final
    {
        return m_function;
    }

    //! \brief Name of the file where the exception was thrown
    //! \details Full path will be returned if parsing fails
    //! \return File name
    virtual std::string FileName (void) const final;

private:
    std::string  m_file;
    RDGE::UInt32 m_line;
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
    explicit SDLException (
                           const std::string& message,
                           std::string        sdl_fn_name,
                           std::string        file           = "",
                           RDGE::UInt32       line           = 0,
                           std::string        parent_fn_name = ""
                          );

    //! \brief SDLException dtor
    virtual ~SDLException (void) { }

    //! \brief Get the SDL function name which failed
    //! \returns Failed SDL function name
    std::string SDLFunction (void) const
    {
        return m_SDLFunction;
    }

    //! \brief Get the SDL generated error message
    //! \return SDL generated error message
    std::string SDLError (void) const
    {
        return m_SDLError;
    }

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
    explicit GLException (
                          const std::string& message,
                          std::string        gl_fn_name,
                          RDGE::UInt32       gl_error_code  = 0,
                          std::string        file           = "",
                          RDGE::UInt32       line           = 0,
                          std::string        parent_fn_name = ""
                         );

    //! \brief GLException dtor
    virtual ~GLException (void) { }

    //! \brief Get the OpenGL function name which failed
    //! \returns Failed OpenGL function name
    std::string GLFunction (void) const
    {
        return m_GLFunction;
    }

    //! \brief Get the OpenGL error code
    //! \return OpenGL error code
    RDGE::UInt32 GLErrorCode (void) const
    {
        return m_GLErrorCode;
    }

    //! \brief Get the OpenGL error code in string format
    //! \return OpenGL error code string
    std::string GLErrorCodeString (void) const;

private:
    std::string  m_GLFunction;
    RDGE::UInt32 m_GLErrorCode;
};

} // namespace Util

// Protmote to RDGE namespace
using Util::Exception;
using Util::SDLException;
using Util::GLException;

} // namespace RDGE
