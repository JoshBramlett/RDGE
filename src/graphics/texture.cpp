#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/shaders/shader_program.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>

#include <cstring> // strrchr
#include <sstream>

namespace rdge {

// Underlying data for texture sharing.  The surface pointer is cached for
// comparison but never dereferenced, because there is no guarantee the
// surface will outlast the texture.  As such, width/height are cached.
struct shared_texture_data
{
    // SDL
    const SDL_Surface* surface; // <---  !!!  DO NOT DEREFERENCE  !!!
    size_t width;
    size_t height;

    // OpenGL
    uint32 handle;
    TextureFilter min_filter;
    TextureFilter mag_filter;
    TextureWrap u_wrap;
    TextureWrap v_wrap;

    // Ref count
    size_t ref_count;
};

class TextureManager
{
public:
    TextureManager (void)
        : m_capacity(ShaderProgram::MaxTextureSlots())
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
        t.min_filter = TextureFilter::NEAREST;
        t.mag_filter = TextureFilter::NEAREST;
        t.u_wrap = TextureWrap::CLAMP_TO_EDGE;
        t.v_wrap = TextureWrap::CLAMP_TO_EDGE;
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
    // requires singleton lazy loading b/c OpenGL context must be established
    static TextureManager mgr;
    return mgr;
}

} // anonymous namespace


Texture::Texture (const std::string& path)
    : Texture(Surface(path))
{ }

Texture::Texture (const Surface& surface)
{
    m_data = GetManager().Register(static_cast<const SDL_Surface*>(surface));
    if (m_data->ref_count == 1)
    {
        Upload(surface);
    }
}

Texture::Texture (const Surface& surface, TextureFilter min, TextureFilter mag)
{
    m_data = GetManager().Register(static_cast<const SDL_Surface*>(surface));
    if (m_data->ref_count == 1)
    {
        m_data->min_filter = min;
        m_data->mag_filter = mag;
        Upload(surface);
    }
    else
    {
        SetFilter(min, mag);
    }
}

Texture::Texture (const Surface& surface, TextureWrap u, TextureWrap v)
{
    m_data = GetManager().Register(static_cast<const SDL_Surface*>(surface));
    if (m_data->ref_count == 1)
    {
        m_data->u_wrap = u;
        m_data->v_wrap = v;
        Upload(surface);
    }
    else
    {
        SetWrap(u, v);
    }
}

Texture::Texture (const Surface& surface,
                  TextureFilter min, TextureFilter mag,
                  TextureWrap u, TextureWrap v)
{
    m_data = GetManager().Register(static_cast<const SDL_Surface*>(surface));
    if (m_data->ref_count == 1)
    {
        m_data->min_filter = min;
        m_data->mag_filter = mag;
        m_data->u_wrap = u;
        m_data->v_wrap = v;
        Upload(surface);
    }
    else
    {
        SetFilter(min, mag);
        SetWrap(u, v);
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

math::svec2
Texture::Size (void) const noexcept
{
    if (m_data)
    {
        return math::svec2(m_data->width, m_data->height);
    }

    return math::svec2(0, 0);
}

void
Texture::Activate (void) const
{
    SDL_assert(m_data);
    SDL_assert(this->unit_id >= 0 && this->unit_id < ShaderProgram::MaxTextureSlots());

    opengl::SetActiveTexture(GL_TEXTURE0 + this->unit_id);
    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);
}

void
Texture::SetFilter (TextureFilter min, TextureFilter mag)
{
    SDL_assert(m_data);
    if (m_data->min_filter == min && m_data->mag_filter == mag)
    {
        return;
    }

    if (m_data->ref_count > 1)
    {
        WLOG() << "Changing filter for a shared texture."
               << " handle=" << m_data->handle
               << " ref_count=" << m_data->ref_count
               << " old_min=" << m_data->min_filter
               << " new_min=" << min
               << " old_mag=" << m_data->mag_filter
               << " new_mag=" << mag;
    }

    m_data->min_filter = min;
    m_data->mag_filter = mag;

    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_MIN_FILTER,
                                to_underlying(m_data->min_filter));
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_MAG_FILTER,
                                to_underlying(m_data->mag_filter));
    opengl::UnbindTexture(GL_TEXTURE_2D);

    DLOG() << "Texture::SetFilter: "
           << " handle=" << m_data->handle
           << " min=" << m_data->min_filter
           << " mag=" << m_data->mag_filter;
}

void
Texture::SetWrap (TextureWrap u, TextureWrap v)
{
    SDL_assert(m_data);
    if (m_data->ref_count > 1)
    {
        WLOG() << "Changing wrapping for a shared texture."
               << " handle=" << m_data->handle
               << " ref_count=" << m_data->ref_count
               << " old_u=" << m_data->u_wrap
               << " new_u=" << u
               << " old_v=" << m_data->v_wrap
               << " new_v=" << v;
    }

    m_data->u_wrap = u;
    m_data->v_wrap = v;

    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_S,
                                to_underlying(m_data->u_wrap));
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_T,
                                to_underlying(m_data->v_wrap));
    opengl::UnbindTexture(GL_TEXTURE_2D);

    DLOG() << "Texture::SetWrap: "
           << " handle=" << m_data->handle
           << " u=" << m_data->u_wrap
           << " v=" << m_data->v_wrap;
}

void
Texture::Upload (const Surface& surface)
{
    // TODO [00036] Add support for 24bpp (no alpha channel) images.  Gimp doesn't
    //      support it out of the box.  I believe the changes would be to convert
    //      the surface to SDL_PIXELFORMAT_BGR888, and set GL_RGB in the OpenGL call.
    //      To check whether 24 or 32bpp, BytesPerPixel should be 3 or 4.
    //
    //      Previously I was calling surface.ChangePixelFormat() to accommodate
    //      changing an RGB to an RGBA, but it seems ridiculous for a couple reasons:
    //        - It doesn't belong here (should be done in the surface)
    //        - It re-creates the surface from scratch.
    //      I should rather use the stb channel overrides to force an RGBA.
    //
    // There are also OpenGL gotchas regarding 24bpp.  See:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads

    opengl::BindTexture(GL_TEXTURE_2D, m_data->handle);

    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_MIN_FILTER,
                                to_underlying(m_data->min_filter));
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_MAG_FILTER,
                                to_underlying(m_data->mag_filter));

    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_S,
                                to_underlying(m_data->u_wrap));
    opengl::SetTextureParameter(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_T,
                                to_underlying(m_data->v_wrap));

    auto surface_ptr = static_cast<const SDL_Surface*>(surface);
    opengl::UploadTextureData(GL_TEXTURE_2D,
                              GL_RGBA8,
                              surface_ptr->w,
                              surface_ptr->h,
                              surface.GL_PixelFormat(),
                              GL_UNSIGNED_BYTE,
                              surface_ptr->pixels);

    opengl::UnbindTexture(GL_TEXTURE_2D);
}

std::ostream&
operator<< (std::ostream& os, TextureFilter value)
{
    return os << rdge::to_string(value);
}

std::ostream&
operator<< (std::ostream& os, TextureWrap value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (TextureFilter value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(TextureFilter::NEAREST)
        CASE(TextureFilter::LINEAR)
        CASE(TextureFilter::MIPMAP_NEAREST_NEAREST)
        CASE(TextureFilter::MIPMAP_LINEAR_NEAREST)
        CASE(TextureFilter::MIPMAP_NEAREST_LINEAR)
        CASE(TextureFilter::MIPMAP_LINEAR_LINEAR)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

std::string
to_string (TextureWrap value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(TextureWrap::MIRRORED_REPEAT)
        CASE(TextureWrap::CLAMP_TO_EDGE)
        CASE(TextureWrap::REPEAT)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

} // namespace rdge
