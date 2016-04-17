#include <rdge/graphics/gltexture.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

namespace RDGE {
namespace Graphics {

GLTexture::GLTexture (const std::string& file, RDGE::UInt32 unit)
    : m_textureId(0)
    , m_textureUnit(unit)
{
    RDGE::Surface surface(file);
    Init(surface);
}

GLTexture::GLTexture (RDGE::Surface& surface, RDGE::UInt32 unit)
    : m_textureId(0)
    , m_textureUnit(unit)
{
    Init(surface);
}

GLTexture::~GLTexture (void)
{
    glDeleteTextures(1, &m_textureId);
}

void
GLTexture::Bind (void)
{
    OpenGL::SetActiveTexture(GL_TEXTURE0 + m_textureUnit);
    OpenGL::BindTexture(GL_TEXTURE_2D, m_textureId);
}

void
GLTexture::Unbind (void)
{
    OpenGL::UnbindTexture(GL_TEXTURE_2D);
}

void
GLTexture::Init (RDGE::Surface& surface)
{
    int texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    std::cout << "samplers=" << texture_units << std::endl;

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texture_units);
    std::cout << "total samplers=" << texture_units << std::endl;

    // TODO: [00036] Add support for 24bpp (no alpha channel) images.  Gimp doesn't
    //       support it out of the box.  I believe the changes would be to convert
    //       the surface to SDL_PIXELFORMAT_BGR888, and set GL_RGB in the OpenGL call.
    //       To check whether 24 or 32bpp, BytesPerPixel should be 3 or 4.

    // Change pixel format to what OpenGL understands
    surface.ChangePixelFormat(SDL_PIXELFORMAT_ABGR8888);

    m_textureId = OpenGL::CreateTexture();
    OpenGL::BindTexture(GL_TEXTURE_2D, m_textureId);

    // TODO: This is a naive implementation.  The following filters are used to
    //       define how OpenGL will handle the scaling when textures are greater
    //       or less than the size of their target.  GL_NEAREST produces harder
    //       edges than other alternatives, so there needs to be a way to specify
    //       in the ctor which option to take.
    // see http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
    OpenGL::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OpenGL::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto surface_ptr = surface.RawPtr();
    OpenGL::SetTextureData(
                           GL_TEXTURE_2D,
                           GL_RGBA,
                           surface_ptr->w,
                           surface_ptr->h,
                           GL_RGBA,
                           GL_UNSIGNED_BYTE,
                           surface_ptr->pixels
                          );

    OpenGL::UnbindTexture(GL_TEXTURE_2D);
}

} // namespace Graphics
} // namespace RDGE
