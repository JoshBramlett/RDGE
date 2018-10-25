#include <rdge/assets/surface.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/system/types.hpp>
#include <rdge/system/window.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <SDL_assert.h>
#include <GL/glew.h>

#define STBI_MALLOC(x) RDGE_MALLOC(x, rdge::memory_bucket_stbi)
#define STBI_FREE(x) RDGE_FREE(x, rdge::memory_bucket_stbi)
#define STBI_REALLOC(x, n) RDGE_REALLOC(x, n, rdge::memory_bucket_stbi)
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <nothings/stb_image.h>

#include <sstream>

namespace {

using namespace rdge;

struct byte_order_masks
{
    uint32 r_mask;
    uint32 g_mask;
    uint32 b_mask;
    uint32 a_mask;
};

byte_order_masks
GetMasks (int32 depth)
{
    // 24bpp depth represents an RGB image, so the byte order is shifted
    // by one byte to remove the alpha channel
    byte_order_masks masks;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (depth == 24) ? 8 : 0;
    masks.r_mask = 0xff000000 >> shift;
    masks.g_mask = 0x00ff0000 >> shift;
    masks.b_mask = 0x0000ff00 >> shift;
    masks.a_mask = 0x000000ff >> shift;
#else
    masks.r_mask = 0x000000ff;
    masks.g_mask = 0x0000ff00;
    masks.b_mask = 0x00ff0000;
    masks.a_mask = (depth == 24) ? 0 : 0xff000000;
#endif

    return masks;
}

} // anonymous namespace

namespace rdge {

// NOTE stb_image currently only supports RGB(A) ordering (with an
//      exception for iphones).  There is however intentions to add
//      support later (see STBI_ORDER_BGR).  Apple/Windows windows
//      should have a default ARGB format that translates to GL_BGRA,
//      which is the preferred method for OpenGL.
//
//      I think SDL_CreateRGBSurfaceWithFormatFrom is assuming the
//      ordering based on the system, so it may be misappropriating
//      the masks and shifts.
//
//      For now we must force SDL_PIXELFORMAT_ABGR8888 rather than
//      using the default from the window in order to get the
//      correct masks/shifts that result in GL_RGBA from the
//      GL_PixelFormat function (which is how it's loaded by stb_image).
//
//      Either wait for stb_image to support BGR, or maybe SDL has a bug
//      that's been fixed.
//
// https://www.khronos.org/opengl/wiki/Common_Mistakes#Slow_pixel_transfer_performance

Surface::Surface (SDL_Surface* surface)
    : m_surface(surface)
{
    SDL_assert(!m_surface || m_surface->userdata == nullptr);
}

Surface::Surface (const std::string& filepath, int32 desired)
{
    if ((desired != 0) && (desired != STBI_rgb) && (desired != STBI_rgb_alpha))
    {
        std::ostringstream ss;
        ss << "Invalid desired channels for Surface."
           << " desired=" << desired
           << " path=" << filepath;

        RDGE_THROW(ss.str());
    }

    int32 w, h, n;
    void* pixel_data = stbi_load(filepath.c_str(), &w, &h, &n, desired);
    if (RDGE_UNLIKELY(!pixel_data))
    {
        std::ostringstream ss;
        ss << "Surface load failed."
           << " what=" << stbi_failure_reason()
           << " path=" << filepath;

        RDGE_THROW(ss.str());
    }

    int32 channels = (desired == 0) ? n : desired;
    int32 depth = channels * 8; // bits per pixel
    int32 pitch = channels * w; // length of a row of pixels in bytes
    m_surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_data,
                                                   w, h,
                                                   depth, pitch,
                                                   SDL_PIXELFORMAT_ABGR8888);
                                                   //Window::Current().PixelFormat());
    if (RDGE_UNLIKELY(!m_surface))
    {
        stbi_image_free(pixel_data);
        SDL_THROW("Failed to create surface from pixel data",
                  "SDL_CreateRGBSurfaceWithFormatFrom");
    }

    m_surface->userdata = pixel_data;
}

Surface::Surface (void* pixel_data, int32 w, int32 h, int32 channels)
{
    if ((channels != STBI_rgb) && (channels != STBI_rgb_alpha))
    {
        std::ostringstream ss;
        ss << "Invalid desired channels for Surface."
           << " desired=" << channels;

        RDGE_THROW(ss.str());
    }

    int32 depth = channels * 8; // bits per pixel
    int32 pitch = channels * w; // length of a row of pixels in bytes
    m_surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_data,
                                                   w, h,
                                                   depth, pitch,
                                                   SDL_PIXELFORMAT_ABGR8888);
                                                   //Window::Current().PixelFormat());
    if (RDGE_UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to create surface from pixel data",
                  "SDL_CreateRGBSurfaceWithFormatFrom");
    }

    m_surface->userdata = pixel_data;
}

Surface::~Surface (void) noexcept
{
    // From SDL docs: https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    // Underlying pixel data is unmanaged and must be freed after the surface.
    //
    // Surfaces that are created using pre-allocated pixel data, we store the
    // pointer to the pixel data in the 'userdata' field.  In all cases the
    // 'userdata' field will point to the same location as the 'pixels' field,
    // but the extra step is required because not all surfaces are created from
    // pre-allocated pixel data.

    if (m_surface)
    {
        void* pixel_data = m_surface->userdata;
        SDL_FreeSurface(m_surface);
        stbi_image_free(pixel_data);
    }
}

bool
Surface::IsEmpty (void) const noexcept
{
    return (m_surface == nullptr);
}

size_t
Surface::Width (void) const noexcept
{
    SDL_assert(m_surface != nullptr);
    return static_cast<size_t>(m_surface->w);
}

size_t
Surface::Height (void) const noexcept
{
    SDL_assert(m_surface != nullptr);
    return static_cast<size_t>(m_surface->h);
}

math::uivec2
Surface::Size (void) const noexcept
{
    SDL_assert(m_surface != nullptr);
    return { static_cast<uint32>(m_surface->w), static_cast<uint32>(m_surface->h) };
}

int32
Surface::Depth (void) const noexcept
{
    SDL_assert(m_surface != nullptr);
    return m_surface->format->BitsPerPixel;
}

uint32
Surface::SDL_PixelFormat (void) const noexcept
{
    SDL_assert(m_surface != nullptr);
    return m_surface->format->format;
}

int32
Surface::GL_PixelFormat (void) const
{
    auto format = m_surface->format;
    if (format->BytesPerPixel == 4)
    {
        if (format->Rshift == 24 && format->Aloss == 0)
        {
            return GL_ABGR_EXT;
        }
        else if (format->Rshift == 16 && format->Aloss == 8)
        {
            return GL_BGRA;
        }
        else if (format->Rshift == 16 && format->Ashift == 24)
        {
            return GL_BGRA;
        }
        else if (format->Rshift == 0 && format->Ashift == 24)
        {
            return GL_RGBA;
        }
    }
    else if (format->BytesPerPixel == 3)
    {
        if (format->Rshift == 16)
        {
            return GL_BGR;
        }
        else if (format->Rshift == 0)
        {
            return GL_RGB;
        }
    }

    ELOG() << "Surface cannot generate OpenGL pixel format."
           << " bpp=" << (uint32)format->BytesPerPixel
           << " rshift=" << (uint32)format->Rshift
           << " ashift=" << (uint32)format->Ashift
           << " aloss=" << (uint32)format->Aloss;
    RDGE_THROW("Pixel Format not recognized for OpenGL display");
}

void
Surface::ChangePixelFormat (uint32 pixel_format)
{
    SDL_assert(m_surface != nullptr);

    if (m_surface->format->format == pixel_format)
    {
        // no change, return
        return;
    }

    // SDL_ConvertSurfaceFormat will create a new surface, so we'll replace
    // our managed surface with the new one if successful and make sure to
    // free the prior surface.

    // last param is 'flags', docs say it's unused and should be set to 0
    auto new_surface = SDL_ConvertSurfaceFormat(m_surface, pixel_format, 0);
    if (RDGE_UNLIKELY(!new_surface))
    {
        SDL_THROW("Failed to convert surface pixel format", "SDL_ConvertSurfaceFormat");
    }

    void* pixel_data = m_surface->userdata;
    SDL_FreeSurface(m_surface);
    m_surface = new_surface;
    m_surface->userdata = pixel_data;
}

Surface
Surface::CreateSubSurface (const screen_rect& clip)
{
    int32 depth = Depth();
    auto masks = GetMasks(depth);
    auto s = SDL_CreateRGBSurface(0, // flags (SDL docs say param is unused)
                                  static_cast<int32>(clip.w),
                                  static_cast<int32>(clip.h),
                                  depth,
                                  masks.r_mask,
                                  masks.g_mask,
                                  masks.b_mask,
                                  masks.a_mask);
    if (RDGE_UNLIKELY(!s))
    {
        SDL_THROW("Failed to create blank surface", "SDL_CreateRGBSurface");
    }

    if (RDGE_UNLIKELY(SDL_BlitSurface(m_surface, &clip, s, nullptr) != 0))
    {
        SDL_THROW("Failed to create sub-surface", "SDL_BlitSurface");
    }

    return Surface(s);
}

} // namespace rdge
