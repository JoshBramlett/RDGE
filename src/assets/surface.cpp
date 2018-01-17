#include <rdge/assets/surface.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/system/types.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <SDL_assert.h>

#define STBI_MALLOC(x) RDGE_MALLOC_SZ(x, nullptr)
#define STBI_FREE(x) RDGE_FREE(x, nullptr)
#define STBI_REALLOC(x, n) RDGE_REALLOC(x, n, nullptr)
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
GetMasks (PixelDepth depth)
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

int32
GetFormat (PixelDepth depth)
{
    // TODO Format is hard-coded.  When adding 24BPP support make sure to query
    //      the windows pixel format (SDL_GetWindowPixelFormat)

    if (depth == PixelDepth::BPP_24)
    {
        return SDL_PIXELFORMAT_BGR24;
    }
    else if (depth == PixelDepth::BPP_32)
    {
        return SDL_PIXELFORMAT_ABGR8888;
    }

    return SDL_PIXELFORMAT_UNKNOWN;
}

} // anonymous namespace

namespace rdge {

Surface::Surface (SDL_Surface* surface)
    : m_surface(surface)
{
    SDL_assert(!m_surface || m_surface->userdata == nullptr);
}

Surface::Surface (const std::string& filepath, PixelDepth depth)
{
    // use number of channels from the file if no override is provided
    int32 channels = 0;
    if (depth == PixelDepth::BPP_24)
    {
        channels = STBI_rgb;
    }
    else if (depth == PixelDepth::BPP_32)
    {
        channels = STBI_rgb_alpha;
    }

    // TODO when implementing a memory tracker, stbi has a define rule to use
    //      a custom allocator.  Look into the same for SDL.

    int32 w, h, file_channels;
    void* pixel_data = stbi_load(filepath.c_str(), &w, &h, &file_channels, channels);
    if (UNLIKELY(!pixel_data))
    {
        std::ostringstream ss;
        ss << "Surface load failed."
           << " what=" << stbi_failure_reason()
           << " path=" << filepath;

        RDGE_THROW(ss.str());
    }

    if (channels == 0)
    {
        channels = file_channels;
    }
    else if (file_channels != channels)
    {
        ILOG() << "Surface format overridden."
               << " file_channels=" << file_channels
               << " channels=" << channels
               << " path=" << filepath;
    }

    if (channels == STBI_rgb)
    {
        depth = PixelDepth::BPP_24;
    }
    else if (channels == STBI_rgb_alpha)
    {
        depth = PixelDepth::BPP_32;
    }
    else
    {
        std::ostringstream ss;
        ss << "Surface format invalid."
           << " channels=" << channels
           << " depth=" << depth
           << " path=" << filepath;

        stbi_image_free(pixel_data);
        RDGE_THROW(ss.str());
    }

    int32 pitch = channels * w; // length of a row of pixels in bytes
    m_surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_data,
                                                   w, h,
                                                   static_cast<int32>(depth),
                                                   pitch,
                                                   GetFormat(depth));
    if (UNLIKELY(!m_surface))
    {
        stbi_image_free(pixel_data);
        SDL_THROW("Failed to create surface from pixel data",
                  "SDL_CreateRGBSurfaceWithFormatFrom");
    }

    m_surface->userdata = pixel_data;
}

Surface::Surface (void* pixel_data, int32 w, int32 h, int32 channels)
{
    PixelDepth depth = PixelDepth::UNKNOWN;
    if (channels == STBI_rgb)
    {
        depth = PixelDepth::BPP_24;
    }
    else if (channels == STBI_rgb_alpha)
    {
        depth = PixelDepth::BPP_32;
    }
    else
    {
        std::ostringstream ss;
        ss << "Surface format invalid."
           << " channels=" << channels
           << " depth=" << depth;

        RDGE_THROW(ss.str());
    }

    int32 pitch = channels * w; // length of a row of pixels in bytes
    m_surface = SDL_CreateRGBSurfaceWithFormatFrom(pixel_data,
                                                   w, h,
                                                   static_cast<int32>(depth),
                                                   pitch,
                                                   GetFormat(depth));
    if (UNLIKELY(!m_surface))
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

    if (m_surface)
    {
        m_surface->refcount--;
        if (m_surface->refcount == 0)
        {
            void* pixel_data = m_surface->userdata;
            SDL_FreeSurface(m_surface);
            RDGE_FREE(pixel_data, nullptr);
        }
    }
}

Surface::Surface (const Surface& other)
    : m_surface(other.m_surface)
{
    if (m_surface)
    {
        m_surface->refcount++;
    }
}

Surface&
Surface::operator= (const Surface& rhs)
{
    m_surface = rhs.m_surface;
    if (m_surface)
    {
        m_surface->refcount++;
    }

    return *this;
}

Surface::Surface (Surface&& other) noexcept
    : m_surface(other.m_surface)
{
    other.m_surface = nullptr;
}

Surface&
Surface::operator= (Surface&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_surface, rhs.m_surface);
    }

    return *this;
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
    auto masks = GetMasks(depth);
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

std::ostream& operator<< (std::ostream& os, PixelDepth value)
{
    if (value == PixelDepth::UNKNOWN)
    {
        os << "unknown";
    }
    else
    {
        os << to_underlying(value) << "bpp";
    }

    return os;
}

} // namespace rdge
