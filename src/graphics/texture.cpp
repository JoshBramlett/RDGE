#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <sstream>

namespace rdge {

// Underlying data for texture sharing.  The surface pointer is cached for
// comparison but never dereferenced, because there is no guarantee the
// surface will outlast the texture.  As such, width/height are cached.
struct shared_texture_data
{
    const SDL_Surface* surface; // DO NOT DEREFERENCE
    size_t width;
    size_t height;
    uint32 handle;              // OpenGL handle
    size_t ref_count;
};

class TextureManager
{
public:
    TextureManager (void)
        : m_capacity(Shader::MaxFragmentShaderUnits())
    {
        if (RDGE_UNLIKELY(!RDGE_TCALLOC(m_textures, m_capacity, memory_bucket_graphics)))
        {
            RDGE_THROW("Failed to allocate memory");
        }
    }

    ~TextureManager (void) noexcept
    {
        RDGE_FREE(m_textures, memory_bucket_graphics);
    }

    shared_texture_data* Register (const SDL_Surface* surface)
    {
        for (size_t i = 0; i < m_count; i++)
        {
            auto& t = m_textures[i];
            if (t.surface == surface)
            {
                t.ref_count++;
                DLOG() << "Texture Reference Added"
                       << " handle=" << t.handle
                       << " ref_count=" << t.ref_count;

                return &t;
            }
        }

        if (++m_count > m_capacity)
        {
            std::ostringstream ss;
            ss << "Failed to register texture.  Max fragment units reached."
               << " limit=" << m_capacity;
            RDGE_THROW(ss.str());
        }

        size_t index = 0;
        for (size_t i = 0; i < m_capacity; i++)
        {
            // find the first available - units can be released in any order
            if (m_textures[i].handle == 0)
            {
                index = i;
                break;
            }
        }

        auto& t = m_textures[index];
        t.surface = surface;
        t.width = surface->w;
        t.height = surface->h;
        t.handle = opengl::CreateTexture();
        t.ref_count = 1;

        ILOG() << "Texture[" << t.width << "x" << t.height << "] Registered"
               << " handle=" << t.handle;

        return &t;
    }

    void Release (shared_texture_data* t)
    {
        if (t)
        {
            SDL_assert(t->ref_count > 0);
            t->ref_count--;
            if (t->ref_count == 0)
            {
                ILOG() << "Texture[" << t->width << "x" << t->height << "] Deleted"
                       << " handle=" << t->handle;

                opengl::DeleteTexture(t->handle);
                t->surface = nullptr;
                t->handle = 0;
                t->ref_count = 0;
                t->width = 0;
                t->height = 0;
            }
            else
            {
                DLOG() << "Texture Reference Removed"
                       << " handle=" << t->handle
                       << " ref_count=" << t->ref_count;
            }
        }
    }

private:
    shared_texture_data* m_textures = nullptr;
    size_t m_count = 0;
    size_t m_capacity = 0;
};

namespace {

TextureManager&
GetManager (void)
{
    // requires singleton lazy loading b/c context must be established
    static TextureManager mgr;
    return mgr;
}

} // anonymous namespace


// TODO Test surface exception can bubble up
Texture::Texture (const std::string& path)
    : Texture(Surface(path))
{ }

Texture::Texture (Surface&& surface)
    : Texture(surface)
{ }

Texture::Texture (Surface& surface)
{
    m_data = GetManager().Register(static_cast<const SDL_Surface*>(surface));
    if (m_data->ref_count == 1)
    {
        Reload(surface);
    }
}

Texture::~Texture (void) noexcept
{
    GetManager().Release(m_data);
}

Texture::Texture (const Texture& other)
    : unit_id(other.unit_id)
    , m_data(other.m_data)
{
    if (m_data)
    {
        SDL_assert(m_data->ref_count > 0);
        m_data->ref_count++;
    }
}

Texture&
Texture::operator= (const Texture& rhs)
{
    if (this != &rhs)
    {
        m_data = rhs.m_data;
        unit_id = rhs.unit_id;

        if (m_data)
        {
            SDL_assert(m_data->ref_count > 0);
            m_data->ref_count++;
        }
    }

    return *this;
}

Texture::Texture (Texture&& other) noexcept
    : unit_id(other.unit_id)
{
    std::swap(m_data, other.m_data);
    other.unit_id = INVALID_UNIT_ID;
}

Texture&
Texture::operator= (Texture&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_data, rhs.m_data);
        unit_id = rhs.unit_id;

        rhs.unit_id = INVALID_UNIT_ID;
    }

    return *this;
}

bool
Texture::operator== (const Texture& other) const noexcept
{
    return (m_data == other.m_data);
}

bool
Texture::operator!= (const Texture& other) const noexcept
{
    return !(*this == other);
}

bool
Texture::IsEmpty (void) const noexcept
{
    return (m_data == nullptr);
}

bool
Texture::IsUnique (void) const noexcept
{
    return (m_data == nullptr) || (m_data->ref_count == 1);
}

size_t
Texture::Width (void) const noexcept
{
    return ((m_data == nullptr) ? 0 : m_data->width);
}

size_t
Texture::Height (void) const noexcept
{
    return ((m_data == nullptr) ? 0 : m_data->height);
}

math::uivec2
Texture::Size (void) const noexcept
{
    if (m_data)
    {
        return math::uivec2(m_data->width, m_data->height);
    }

    return math::uivec2(0, 0);
}

void
Texture::Activate (void) const
{
    SDL_assert(m_data);
    SDL_assert(this->unit_id >= 0 && this->unit_id < Shader::MaxFragmentShaderUnits());

    opengl::SetActiveTexture(GL_TEXTURE0 + this->unit_id);
    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);
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

    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);

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
}

} // namespace rdge
