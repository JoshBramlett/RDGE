#include <rdge/gfx/texture.hpp>
#include <rdge/gfx/shaders/shader.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

using namespace rdge;
using namespace rdge::gfx;

Texture::Texture (void)
    : m_textureId(0)
    , m_textureUnitId(-1)
    , m_width(0)
    , m_height(0)
{ }

Texture::Texture (const std::string& file)
    : m_textureId(0)
    , m_textureUnitId(-1)
    , m_width(0)
    , m_height(0)
{
    // Allow surface exception to bubble up if unavailable
    auto surface = std::make_shared<rdge::Surface>(file);

    m_textureId = opengl::CreateTexture();
    ResetData(surface);
}

Texture::Texture (std::shared_ptr<rdge::Surface>& surface)
    : m_textureId(0)
    , m_textureUnitId(-1)
    , m_width(0)
    , m_height(0)
{
    m_textureId = opengl::CreateTexture();
    ResetData(surface);
}

Texture::~Texture (void)
{
    glDeleteTextures(1, &m_textureId);
}

void
Texture::SetUnitID (rdge::int32 id)
{
    m_textureUnitId = id;
}

void
Texture::Activate (void) const
{
    SDL_assert(m_textureUnitId >= 0 && m_textureUnitId < Shader::MaxFragmentShaderUnits());

    opengl::SetActiveTexture(GL_TEXTURE0 + m_textureUnitId);
    opengl::BindTexture(GL_TEXTURE_2D, m_textureId);
}

void
Texture::ResetData (std::shared_ptr<rdge::Surface>& surface)
{
    // TODO: [00036] Add support for 24bpp (no alpha channel) images.  Gimp doesn't
    //       support it out of the box.  I believe the changes would be to convert
    //       the surface to SDL_PIXELFORMAT_BGR888, and set GL_RGB in the OpenGL call.
    //       To check whether 24 or 32bpp, BytesPerPixel should be 3 or 4.

    // Change pixel format to what OpenGL understands
    surface->ChangePixelFormat(SDL_PIXELFORMAT_ABGR8888);
    m_width = surface->Width();
    m_height = surface->Height();

    opengl::BindTexture(GL_TEXTURE_2D, m_textureId);

    // TODO: This is a naive implementation.  The following filters are used to
    //       define how OpenGL will handle the scaling when textures are greater
    //       or less than the size of their target.  GL_NEAREST produces harder
    //       edges than other alternatives, so there needs to be a way to specify
    //       in the ctor which option to take.
    // see http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
    opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //auto surface_ptr = surface->RawPtr();
    auto surface_ptr = static_cast<const SDL_Surface*>(*surface);
    opengl::SetTextureData(
                           GL_TEXTURE_2D,
                           GL_RGBA,
                           surface_ptr->w,
                           surface_ptr->h,
                           GL_RGBA,
                           GL_UNSIGNED_BYTE,
                           surface_ptr->pixels
                          );

    opengl::UnbindTexture(GL_TEXTURE_2D);
}
