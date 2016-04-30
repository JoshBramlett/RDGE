#include <rdge/assets/surface.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_image.h>

namespace RDGE {
namespace Assets {

Surface::Surface (SDL_Surface* surface)
    : m_surface(surface)
    , m_pixelData(nullptr)
{ }

Surface::Surface (const std::string& file)
    : m_pixelData(nullptr)
{
    m_surface = IMG_Load(file.c_str());
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to load surface. file=" + file, "IMG_Load");
    }
}

Surface::Surface (
                  RDGE::Int32  width,
                  RDGE::Int32  height,
                  RDGE::Int32  depth,
                  RDGE::UInt32 r_mask,
                  RDGE::UInt32 g_mask,
                  RDGE::UInt32 b_mask,
                  RDGE::UInt32 a_mask
                 )
    : m_pixelData(nullptr)
{
    m_surface = SDL_CreateRGBSurface(
                                     0, // flags (SDL docs say param is unused)
                                     width,
                                     height,
                                     depth,
                                     r_mask,
                                     g_mask,
                                     b_mask,
                                     a_mask
                                    );
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to create blank surface", "SDL_CreateRGBSurface");
    }
}

Surface::Surface (
                  std::unique_ptr<unsigned char[]> pixels,
                  RDGE::Int32                      width,
                  RDGE::Int32                      height,
                  RDGE::Int32                      depth,
                  RDGE::Int32                      pitch,
                  RDGE::UInt32                     r_mask,
                  RDGE::UInt32                     g_mask,
                  RDGE::UInt32                     b_mask,
                  RDGE::UInt32                     a_mask
                 )
    : m_pixelData(std::move(pixels))
{
    m_surface = SDL_CreateRGBSurfaceFrom(
                                         pixels.get(),
                                         width,
                                         height,
                                         depth,
                                         pitch,
                                         r_mask,
                                         g_mask,
                                         b_mask,
                                         a_mask
                                        );
    if (UNLIKELY(!m_surface))
    {
        SDL_THROW("Failed to create surface from pixels", "SDL_CreateRGBSurfaceFrom");
    }
}

Surface::~Surface (void)
{
    if (m_surface != nullptr)
    {
        SDL_FreeSurface(m_surface);
    }
}

Surface::Surface (Surface&& rhs) noexcept
    : m_surface(rhs.m_surface)
    , m_pixelData(std::move(rhs.m_pixelData))
{
    rhs.m_surface = nullptr;
}

Surface&
Surface::operator= (Surface&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_surface != nullptr)
        {
            SDL_FreeSurface(m_surface);
        }

        m_surface = rhs.m_surface;
        m_pixelData = std::move(rhs.m_pixelData);
        rhs.m_surface = nullptr;
    }

    return *this;
}

RDGE::UInt32
Surface::Width (void) const noexcept
{
    if (!m_surface)
    {
        return 0;
    }

    return static_cast<RDGE::UInt32>(m_surface->w);
}

RDGE::UInt32
Surface::Height (void) const noexcept
{
    if (!m_surface)
    {
        return 0;
    }

    return static_cast<RDGE::UInt32>(m_surface->h);
}

RDGE::Graphics::Size
Surface::Size (void) const noexcept
{
    if (!m_surface)
    {
        return RDGE::Graphics::Size(0, 0);
    }

    return RDGE::Graphics::Size(m_surface->w, m_surface->h);
}

RDGE::UInt32
Surface::PixelFormat (void) const noexcept
{
    if (!m_surface)
    {
        return SDL_PIXELFORMAT_UNKNOWN;
    }

    return m_surface->format->format;
}

void
Surface::ChangePixelFormat (RDGE::UInt32 pixel_format)
{
    if (!m_surface)
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

} // namespace Assets
} // namespace RDGE
