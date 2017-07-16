#include <rdge/assets/surface.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

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
GetMasksFromDepth (PixelDepth depth)
{
    // 24bpp depth represents an RGB image, so the byte order is shifted
    // by one byte to remove the alpha channel
    byte_order_masks masks;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (depth == PixelDepth::BPP_24) ? 8 : 0;
    masks.r_mask = 0xff000000 >> shift;
    masks.g_mask = 0x00ff0000 >> shift;
    masks.b_mask = 0x0000ff00 >> shift;
    masks.a_mask = 0x000000ff >> shift;
#else
    masks.r_mask = 0x000000ff;
    masks.g_mask = 0x0000ff00;
    masks.b_mask = 0x00ff0000;
    masks.a_mask = (depth == PixelDepth::BPP_24) ? 0 : 0xff000000;
#endif

    return masks;
}

} // anonymous namespace

namespace rdge {

Surface::Surface (SDL_Surface* surface)
    : m_surface(surface)
{
    // TODO This wrapper could be smarter (and safer from double frees) if
    //      support is added for using the SDL_Surface refcount property.
}

Surface::Surface (const std::string& path, PixelDepth depth)
{
    int32 req_format = 0;
    if (depth == PixelDepth::BPP_24)
    {
        req_format = STBI_rgb;
    }
    else if (depth == PixelDepth::BPP_32)
    {
        req_format = STBI_rgb_alpha;
    }

    int32 w, h, orig_format;
    m_data = stbi_load(path.c_str(), &w, &h, &orig_format, req_format);
    if (UNLIKELY(!m_data))
    {
        std::ostringstream ss;
        ss << "Surface load failed."
           << " what=" << stbi_failure_reason()
           << " path=" << path;

        RDGE_THROW(ss.str());
    }

    if (req_format == 0)
    {
        req_format = orig_format;
    }
    else if (orig_format != req_format)
    {
        std::ostringstream ss;
        ss << "Surface format overridden."
           << " orig=" << orig_format
           << " req=" << req_format
           << " path=" << path;

        ILOG(ss.str());
    }

    // TODO Format is hard-coded.  When adding 24BPP support make sure to query
    //      the windows pixel format (SDL_GetWindowPixelFormat)

    int32 sdl_pixel_format;
    int32 pitch; // length of a row of pixels in bytes
    if (req_format == STBI_rgb)
    {
        depth = PixelDepth::BPP_24;
        sdl_pixel_format = SDL_PIXELFORMAT_BGR24;
        pitch = 3 * w;
    }
    else if (req_format == STBI_rgb_alpha)
    {
        depth = PixelDepth::BPP_32;
        sdl_pixel_format = SDL_PIXELFORMAT_ABGR8888;
        pitch = 4 * w;
    }
    else
    {
        std::ostringstream ss;
        ss << "Surface format invalid."
           << " req=" << req_format
           << " depth=" << static_cast<int32>(depth)
           << " path=" << path;

        stbi_image_free(m_data);
        RDGE_THROW(ss.str());
    }

    m_surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)m_data,
                                         w, h,
                                         static_cast<int32>(depth),
                                         pitch,
                                         sdl_pixel_format);
    if (UNLIKELY(!m_surface))
    {
        stbi_image_free(m_data);
        SDL_THROW("Failed to create surface from pixel data", "SDL_CreateRGBSurfaceFrom");
    }

    if (!math::is_pot(m_surface->w) || !math::is_pot(m_surface->h))
    {
        std::ostringstream ss;
        ss << "Surface loaded has NPOT dimensions."
           << " w=" << m_surface->w
           << " h=" << m_surface->h
           << " path=" << path;

        WLOG(ss.str());
    }
}

Surface::~Surface (void) noexcept
{
    // From SDL docs: https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    // Underlying pixel data is unmanaged and must be freed after the surface.

    SDL_FreeSurface(m_surface);
    stbi_image_free(m_data);
}

Surface::Surface (Surface&& rhs) noexcept
    : m_data(rhs.m_data)
    , m_surface(rhs.m_surface)
{
    rhs.m_surface = nullptr;
    rhs.m_data = nullptr;
}

Surface&
Surface::operator= (Surface&& rhs) noexcept
{
    if (this != &rhs)
    {
        SDL_FreeSurface(m_surface);
        stbi_image_free(m_data);

        m_surface = rhs.m_surface;
        m_data = rhs.m_data;
        rhs.m_surface = nullptr;
        rhs.m_data = nullptr;
    }

    return *this;
}

uint32
Surface::Width (void) const noexcept
{
    SDL_assert(m_surface != nullptr);

    return static_cast<uint32>(m_surface->w);
}

uint32
Surface::Height (void) const noexcept
{
    SDL_assert(m_surface != nullptr);

    return static_cast<uint32>(m_surface->h);
}

math::uivec2
Surface::Size (void) const noexcept
{
    SDL_assert(m_surface != nullptr);

    return { static_cast<uint32>(m_surface->w), static_cast<uint32>(m_surface->h) };
}

uint32
Surface::PixelFormat (void) const noexcept
{
    SDL_assert(m_surface != nullptr);

    return m_surface->format->format;
}

PixelDepth
Surface::Depth (void) const noexcept
{
    SDL_assert(m_surface != nullptr);

    return static_cast<PixelDepth>(m_surface->format->BitsPerPixel);
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
    if (UNLIKELY(!new_surface))
    {
        SDL_THROW("Failed to convert surface pixel format", "SDL_ConvertSurfaceFormat");
    }

    SDL_FreeSurface(m_surface);
    m_surface = new_surface;
}

Surface
Surface::CreateSubSurface (const screen_rect& clip)
{
    auto depth = Depth();
    auto masks = GetMasksFromDepth(depth);
    auto s = SDL_CreateRGBSurface(0, // flags (SDL docs say param is unused)
                                  static_cast<int32>(clip.w),
                                  static_cast<int32>(clip.h),
                                  static_cast<int32>(depth),
                                  masks.r_mask,
                                  masks.g_mask,
                                  masks.b_mask,
                                  masks.a_mask);
    if (UNLIKELY(!s))
    {
        SDL_THROW("Failed to create blank surface", "SDL_CreateRGBSurface");
    }

    if (UNLIKELY(SDL_BlitSurface(m_surface, &clip, s, nullptr) != 0))
    {
        SDL_THROW("Failed to create sub-surface", "SDL_BlitSurface");
    }

    return Surface(s);
}

} // namespace rdge
