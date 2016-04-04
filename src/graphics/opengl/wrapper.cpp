#include <rdge/graphics/opengl/wrapper.hpp>
#include <rdge/internal/exception_macros.hpp>

namespace RDGE {
namespace Graphics {
namespace OpenGL {

void
gl_throw_on_error (const char* func)
{
    GLenum code = glGetError();
    if (code != GL_NO_ERROR)
    {
        GL_THROW("OpenGL call failed", func, static_cast<RDGE::UInt32>(code));
    }
}

} // namespace OpenGL
} // namespace Graphics
} // namespace RDGE
