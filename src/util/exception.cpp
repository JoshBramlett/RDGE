#include <rdge/util/exception.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL_error.h>

#include <memory>

namespace rdge {

Exception::Exception (const std::string& message,
                      std::string        file,
                      uint32             line,
                      std::string        fn_name)
    : std::runtime_error(message)
    , m_file(std::move(file))
    , m_line(line)
    , m_function(std::move(fn_name))
{ }

std::string
Exception::FileName (void) const
{
    return rdge::basename(m_file);
}

SDLException::SDLException (const std::string& message,
                            std::string        sdl_fn_name,
                            std::string        file,
                            uint32             line,
                            std::string        parent_fn_name)
    : Exception(message, std::move(file), line, std::move(parent_fn_name))
    , m_SDLFunction(std::move(sdl_fn_name))
    , m_SDLError(SDL_GetError())
{ }

GLException::GLException (const std::string& message,
                          std::string        gl_fn_name,
                          uint32             gl_error_code,
                          std::string        file,
                          uint32             line,
                          std::string        parent_fn_name)
    : Exception(message, std::move(file), line, std::move(parent_fn_name))
    , m_GLFunction(std::move(gl_fn_name))
    , m_GLErrorCode(gl_error_code)
{ }

std::string
GLException::GLErrorCodeString (void) const
{
    return opengl::ErrorCodeToString(m_GLErrorCode);
}

} // namespace rdge
