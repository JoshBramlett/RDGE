#include <rdge/assets/surface.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_image.h>

/* Saving the default masks (which were in the header file) in case I revert
 * the ctors back to being simple wrappers for SDL_CreateRGBSurface and
 * SDL_CreateRGBSurfaceFrom

//!@{
//! \brief Default byte order determined from machine endianess
//! \details Pixels are interpreted as a 32 bit integer, where each byte is a
//!          color (or alpha).  For example, on a machine with a high byte
//!          order (big endian), red is stored in the most significant byte.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    static constexpr rdge::uint32 DEFAULT_RMASK = 0xff000000;
    static constexpr rdge::uint32 DEFAULT_GMASK = 0x00ff0000;
    static constexpr rdge::uint32 DEFAULT_BMASK = 0x0000ff00;
    static constexpr rdge::uint32 DEFAULT_AMASK = 0x000000ff;
#else
    static constexpr rdge::uint32 DEFAULT_RMASK = 0x000000ff;
    static constexpr rdge::uint32 DEFAULT_GMASK = 0x0000ff00;
    static constexpr rdge::uint32 DEFAULT_BMASK = 0x00ff0000;
    static constexpr rdge::uint32 DEFAULT_AMASK = 0xff000000;
#endif
//!@}
*/

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
    masks.a_mask = (depth == PixelDepth::BPP_24) ? 0 : 0x000000ff;
#endif

    return masks;
}

} // anonymous namespace

namespace rdge {

Surface::Surface (SDL_Surface* surface)
    : m_surface(surface)
{ }

Surface::Surface (const std::string& path)
{
    m_surface = IMG_Load(path.c_str());
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to load surface. file=" + path, "IMG_Load");
    }
}

Surface::Surface (const rdge::gfx::size& size, PixelDepth depth)
{
    auto masks = GetMasksFromDepth(depth);
    m_surface = SDL_CreateRGBSurface(0, // flags (SDL docs say param is unused)
                                     static_cast<int32>(size.w),
                                     static_cast<int32>(size.h),
                                     static_cast<int32>(depth),
                                     masks.r_mask,
                                     masks.g_mask,
                                     masks.b_mask,
                                     masks.a_mask);
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to create blank surface", "SDL_CreateRGBSurface");
    }
}

Surface::Surface (std::unique_ptr<uint8[]> pixels,
                  const rdge::gfx::size&   size,
                  PixelDepth               depth)
    : m_pixelData(std::move(pixels))
{
    if (UNLIKELY(!m_pixelData))
    {
        RDGE_THROW("Pixel data is null");
    }

    auto masks = GetMasksFromDepth(depth);
    auto pitch = static_cast<int32>(size.w * ((depth == PixelDepth::BPP_24) ? 3 : 4));
    m_surface = SDL_CreateRGBSurfaceFrom(reinterpret_cast<void*>(m_pixelData.get()),
                                         static_cast<int32>(size.w),
                                         static_cast<int32>(size.h),
                                         static_cast<int32>(depth),
                                         pitch, // length of a row of pixels in bytes
                                         masks.r_mask,
                                         masks.g_mask,
                                         masks.b_mask,
                                         masks.a_mask);
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to create surface from pixels", "SDL_CreateRGBSurfaceFrom");
    }
}

Surface::~Surface (void) noexcept
{
    // SDL documentation states the underlying pixel data is unmanaged and must
    // be freed after the surface.  The pixel data is wrapped in a unique_ptr, but
    // leaving this note here so I don't do anything crazy in the future.
    // See remarks - https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom

    if (m_surface)
    {
        SDL_FreeSurface(m_surface);
    }
}

Surface::Surface (Surface&& rhs) noexcept
{
    if (m_surface)
    {
        SDL_FreeSurface(m_surface);
    }

    m_pixelData.swap(rhs.m_pixelData);
    m_surface = rhs.m_surface;
    rhs.m_surface = nullptr;
}

Surface&
Surface::operator= (Surface&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_surface)
        {
            SDL_FreeSurface(m_surface);
        }

        m_pixelData.swap(rhs.m_pixelData);
        m_surface = rhs.m_surface;
        rhs.m_surface = nullptr;
    }

    return *this;
}

uint32
Surface::Width (void) const noexcept
{
    if (UNLIKELY(!m_surface))
    {
        return 0;
    }

    return static_cast<uint32>(m_surface->w);
}

uint32
Surface::Height (void) const noexcept
{
    if (UNLIKELY(!m_surface))
    {
        return 0;
    }

    return static_cast<uint32>(m_surface->h);
}

rdge::gfx::size
Surface::Size (void) const noexcept
{
    if (UNLIKELY(!m_surface))
    {
        return rdge::gfx::size(0, 0);
    }

    return rdge::gfx::size(m_surface->w, m_surface->h);
}

uint32
Surface::PixelFormat (void) const noexcept
{
    if (UNLIKELY(!m_surface))
    {
        return SDL_PIXELFORMAT_UNKNOWN;
    }

    return m_surface->format->format;
}

void
Surface::ChangePixelFormat (uint32 pixel_format)
{
    if (UNLIKELY(!m_surface))
    {
        RDGE_THROW("Attempting to change pixel format on a NULL surface");
    }

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
    m_pixelData.reset(nullptr);

    m_surface = new_surface;
}

} // namespace rdge
