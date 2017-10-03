#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

namespace rdge {

Texture::Texture (void)
{ }

// TODO Test surface exception can bubble up
Texture::Texture (const std::string& path)
    : Texture(Surface(path))
{ }

Texture::Texture (Surface&& surface)
    : Texture(surface)
{ }

Texture::Texture (Surface& surface)
{
    auto surface_ptr = static_cast<const SDL_Surface*>(surface);
    if (!math::is_pot(surface_ptr->w) || !math::is_pot(surface_ptr->h))
    {
        WLOG() << "Texture loaded has NPOT dimensions."
               << " w=" << surface_ptr->w
               << " h=" << surface_ptr->h;
    }

    m_handle = opengl::CreateTexture();
    Reload(surface);
}

Texture::~Texture (void) noexcept
{
    glDeleteTextures(1, &m_handle);
}

Texture::Texture (Texture&& rhs) noexcept
    : unit_id(rhs.unit_id)
    , width(rhs.width)
    , height(rhs.height)
{
    std::swap(m_handle, rhs.m_handle);
    rhs.unit_id = INVALID_UNIT_ID;
    rhs.width   = 0;
    rhs.height  = 0;
}

Texture&
Texture::operator= (Texture&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_handle, rhs.m_handle);
        unit_id = rhs.unit_id;
        width   = rhs.width;
        height  = rhs.height;

        rhs.unit_id = INVALID_UNIT_ID;
        rhs.width   = 0;
        rhs.height  = 0;
    }

    return *this;
}

void
Texture::Activate (void) const
{
    SDL_assert(this->unit_id >= 0 && this->unit_id < Shader::MaxFragmentShaderUnits());

    opengl::SetActiveTexture(GL_TEXTURE0 + this->unit_id);
    opengl::BindTexture(GL_TEXTURE_2D, m_handle);
}

void
Texture::Reload (Surface& surface)
{
    // TODO: [00036] Add support for 24bpp (no alpha channel) images.  Gimp doesn't
    //       support it out of the box.  I believe the changes would be to convert
    //       the surface to SDL_PIXELFORMAT_BGR888, and set GL_RGB in the OpenGL call.
    //       To check whether 24 or 32bpp, BytesPerPixel should be 3 or 4.
    //
    //       Removing ChangePixelFormat creates an ASAN crash in 04_scenes b/c it
    //       it loads RGB textures and their isn't proper support.

    // Change pixel format to what OpenGL understands
    // TODO: This should be set to whatever the window uses
    //          SDL_GetWindowPixelFormat(m_window);
    surface.ChangePixelFormat(SDL_PIXELFORMAT_ABGR8888);

    opengl::BindTexture(GL_TEXTURE_2D, m_handle);

    // TODO: This is a naive implementation.  The following filters are used to
    //       define how OpenGL will handle the scaling when textures are greater
    //       or less than the size of their target.  GL_NEAREST produces harder
    //       edges than other alternatives, so there needs to be a way to specify
    //       in the ctor which option to take.
    // see http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
    opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //opengl::SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // TODO The second param is the image precision, so when I do 24bpp support take
    //      into consideration:
    //
    //      Although GL will accept GL_RGB, it is up to the driver to decide an
    //      appropriate precision. We recommend that you be specific and write GL_RGB8:
    //      https://www.opengl.org/wiki/Common_Mistakes#Image_precision
    auto surface_ptr = static_cast<const SDL_Surface*>(surface);
    opengl::SetTextureData(GL_TEXTURE_2D,
                           GL_RGBA,
                           surface_ptr->w,
                           surface_ptr->h,
                           GL_RGBA,
                           GL_UNSIGNED_BYTE,
                           surface_ptr->pixels);

    opengl::UnbindTexture(GL_TEXTURE_2D);

    this->width = surface_ptr->w;
    this->height = surface_ptr->h;
}

bool
Texture::IsEmpty (void) const
{
    return (m_handle == 0);
}

} // namespace rdge
