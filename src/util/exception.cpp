#include <rdge/util/exception.hpp>

#include <SDL.h>

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
                            std::string        fn_name
                           )
    : Exception(message, file, line, fn_name)
    , m_SDLFunction(std::move(sdl_fn_name))
    , m_SDLError(SDL_GetError())
{ }

} // namespace Util
} // namespace RDGE
