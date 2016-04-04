#include <rdge/util/exception.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <memory>

namespace RDGE {
namespace Util {

namespace {
    // returns the text after the last slash, or the full text if
    // no slash is present
    inline std::string GetFileFromPath (const std::string& path)
    {
        const size_t last_slash_idx = path.find_last_of("\\/");
        if (last_slash_idx != std::string::npos)
        {
            return path.substr(last_slash_idx +1);
        }

        return path;
    }
}

Exception::Exception (
                      const std::string& message,
                      std::string        file,
                      RDGE::UInt32       line,
                      std::string        fn_name
                     )
    : std::runtime_error(message)
    , m_file(std::move(file))
    , m_line(line)
    , m_function(std::move(fn_name))
{ }

std::string
Exception::FileName (void) const
{
    return GetFileFromPath(m_file);
}

SDLException::SDLException (
                            const std::string& message,
                            std::string        sdl_fn_name,
                            std::string        file,
                            RDGE::UInt32       line,
                            std::string        parent_fn_name
                           )
    : Exception(message, file, line, parent_fn_name)
    , m_SDLFunction(std::move(sdl_fn_name))
    , m_SDLError(SDL_GetError())
{ }

GLException::GLException (
                          const std::string& message,
                          std::string        gl_fn_name,
                          RDGE::UInt32       gl_error_code,
                          std::string        file,
                          RDGE::UInt32       line,
                          std::string        parent_fn_name
                         )
    : Exception(message, file, line, parent_fn_name)
    , m_GLFunction(std::move(gl_fn_name))
    , m_GLErrorCode(gl_error_code)
{ }

std::string
GLException::GLErrorCodeString (void) const
{
    if (m_GLErrorCode != 0)
    {
        switch(m_GLErrorCode)
        {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:
            return "GL_CONTEXT_LOST";
        default:
            break;
        }
    }

    return "Unknown Error (" + std::to_string(m_GLErrorCode) + ")";
}

} // namespace Util
} // namespace RDGE
