#include <rdge/window.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <algorithm>

namespace RDGE {

namespace {
    /*
     * Frame rate timer
     */

    // Number of frames stored to calculate the moving average
    constexpr RDGE::UInt32 FRAME_SAMPLES = 100;

    RDGE::Util::Timer s_frameTimer;
    RDGE::UInt32      s_tickIndex;
    RDGE::UInt32      s_tickSum;
    RDGE::UInt32      s_tickSamples[FRAME_SAMPLES];

    /*
     * Aspect ratio
     */

    constexpr double FP_5x4   = 1.25;
    constexpr double FP_4x3   = 1.33;
    constexpr double FP_16x10 = 1.60;
    constexpr double FP_16x9  = 1.78;

    AspectRatio
    CalculateAspectRatio (RDGE::UInt32 width, RDGE::UInt32 height)
    {
        double epsilon      = 0.1;
        double aspect_ratio = static_cast<double>(width) / static_cast<double>(height);

        if ((aspect_ratio - FP_5x4) < epsilon)
        {
            return AspectRatio::R5_4;
        }
        else if ((aspect_ratio - FP_4x3) < epsilon)
        {
            return AspectRatio::R4_3;
        }
        else if ((aspect_ratio - FP_16x10) < epsilon)
        {
            return AspectRatio::R16_10;
        }
        else if ((aspect_ratio - FP_16x9) < epsilon)
        {
            return AspectRatio::R16_9;
        }

        return AspectRatio::Invalid;
    }
} // anonymous namespace

Window::Window (void)
    : m_renderer(nullptr)
    , m_window(nullptr)
{ }

Window::Window (
                const std::string& title,
                RDGE::UInt32       width,
                RDGE::UInt32       height,
                bool               use_vsync
               )
    : m_backgroundColor(RDGE::Color::Black())
    , m_aspectRatio(CalculateAspectRatio(width, height))
    , m_renderer(nullptr)
    , m_window(nullptr)
{

    // TODO:  There needs to be an elegant way to instantiate this object,
    //        but having a ton of different params isn't it.  The 'flags'
    //        variable should be removed in the process.
    RDGE::UInt32 flags = 0;

    flags = SDL_WINDOW_SHOWN;
    m_window = SDL_CreateWindow(
                                title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                width,
                                height,
                                flags
                               );
    if (UNLIKELY(!m_window))
    {
        SDL_THROW("SDL failed to create a Window", "SDL_CreateWindow");
    }

    flags = SDL_RENDERER_ACCELERATED;
    if (use_vsync)
    {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    // second param is the index of driver, -1 for first to match the flags
    m_renderer = SDL_CreateRenderer(m_window, -1, flags);
    if (UNLIKELY(!m_renderer))
    {
        SDL_DestroyWindow(m_window);
        SDL_THROW("SDL failed to create a Renderer", "SDL_CreateRenderer");
    }
}

Window::~Window (void)
{
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
    }

    if (m_window != nullptr)
    {
        SDL_DestroyWindow(m_window);
    }
}

Window::Window (Window&& rhs) noexcept
    : m_renderer(rhs.m_renderer)
    , m_window(rhs.m_window)
{
    rhs.m_renderer = nullptr;
    rhs.m_window = nullptr;
}

Window&
Window::operator= (Window&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_renderer != nullptr)
        {
            SDL_DestroyRenderer(m_renderer);
        }

        m_renderer = rhs.m_renderer;
        rhs.m_renderer = nullptr;

        if (m_window != nullptr)
        {
            SDL_DestroyWindow(m_window);
        }

        m_window = rhs.m_window;
        rhs.m_window = nullptr;
    }

    return *this;
}

std::string
Window::Title (void) const
{
    return SDL_GetWindowTitle(m_window);
}

RDGE::Graphics::Size
Window::Size (void) const
{
    RDGE::Int32 width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    return RDGE::Graphics::Size(width, height);
}

RDGE::UInt32
Window::Width (void) const
{
    RDGE::Int32 width;
    SDL_GetWindowSize(m_window, &width, nullptr);

    return static_cast<RDGE::UInt32>(width);
}

RDGE::UInt32
Window::Height (void) const
{
    RDGE::Int32 height;
    SDL_GetWindowSize(m_window, nullptr, &height);

    return static_cast<RDGE::UInt32>(height);
}

void
Window::SetTitle (const std::string& title)
{
    SDL_SetWindowTitle(m_window, title.c_str());
}

void
Window::SetSize (RDGE::UInt32 width, RDGE::UInt32 height)
{
    SDL_SetWindowSize(m_window, width, height);
}

void
Window::SetSize (const RDGE::Graphics::Size& size)
{
    SDL_SetWindowSize(m_window, size.w, size.h);
}

void
Window::SetBackgroundColor (const RDGE::Color& color)
{
    m_backgroundColor = color;
}

void
Window::Clear (void)
{
    // Reset the draw color in case it was set externally
    SDL_SetRenderDrawColor(
                           m_renderer,
                           m_backgroundColor.r,
                           m_backgroundColor.g,
                           m_backgroundColor.b,
                           m_backgroundColor.a
                          );

    SDL_RenderClear(m_renderer);
}

void
Window::Present (void)
{
    SDL_RenderPresent(m_renderer);
}

void
Window::Draw (
              const RDGE::Texture&         texture,
              const RDGE::Graphics::Point& point,
              const RDGE::Graphics::Rect&  clip
             ) const
{
    RDGE::Int32 w = 0, h = 0;
    if (UNLIKELY(SDL_QueryTexture(texture.RawPtr(), NULL, NULL, &w, &h) != 0))
    {
        SDL_THROW("Unable to get texture size", "SDL_QueryTexture");
    }

    auto destination = RDGE::Graphics::Rect(point, RDGE::Graphics::Size(w, h));
    Draw(texture, destination, clip);
}

void
Window::Draw (
              const RDGE::Texture&        texture,
              const RDGE::Graphics::Rect& destination,
              const RDGE::Graphics::Rect& clip
             ) const
{
    if (UNLIKELY(SDL_RenderCopy(m_renderer,
                                texture.RawPtr(),
                                (clip.IsEmpty()) ? nullptr : &clip,
                                &destination
                               ) != 0))
    {
        SDL_THROW("SDL failed to copy texture to renderer", "SDL_RenderCopy");
    }
}

void
Window::DrawPrimitiveRect (
                           RDGE::Graphics::Rect* rect,
                           const RDGE::Color&    color,
                           bool                  fill,
                           SDL_BlendMode         blend_mode
                          ) const
{
    SDL_SetRenderDrawColor(
                           m_renderer,
                           color.r,
                           color.g,
                           color.b,
                           color.a
                          );

    SDL_SetRenderDrawBlendMode(m_renderer, blend_mode);

    if (fill)
    {
        if (UNLIKELY(SDL_RenderFillRect(m_renderer, rect) != 0))
        {
            SDL_THROW("Failed drawing primitive rectangle", "SDL_RenderFillRect");
        }
    }
    else
    {
        if (UNLIKELY(SDL_RenderDrawRect(m_renderer, rect) != 0))
        {
            SDL_THROW("Failed drawing primitive rectangle", "SDL_RenderDrawRect");
        }
    }
}
/*
RDGE::Surface
Window::Screenshot (void)
{
    SDL_RendererInfo info;
    if (UNLIKELY(SDL_GetRendererInfo(m_renderer, &info) != 0))
    {
        SDL_THROW("Failed to get renderer info", "SDL_GetRendererInfo");
    }

    // We store the screenshot in an empty RGB surface, so we need to look for a
    // compatible texture format from those available through our renderer.
    // SDL_CreateRGBSurface uses SDL_MasksToPixelFormatEnum to validate the masks,
    // so we'll call the opposite SDL_PixelFormatEnumToMasks to validate the
    // format.  For all valid formats, we then prefer the one with the largest depth

    // vars required to build surface
    RDGE::Int32 bpp = 0;
    RDGE::UInt32 format_index;
    RDGE::UInt32 r, g, b, a;

    // out param temp placeholder vars
    RDGE::Int32 t_bpp;
    RDGE::UInt32 t_r, t_g, t_b, t_a;
    for (RDGE::UInt32 i = 0; i < info.num_texture_formats; ++i)
    {
        if (SDL_PixelFormatEnumToMasks(info.texture_formats[i], &t_bpp, &t_r, &t_g, &t_b, &t_a))
        {
            if (t_bpp > bpp)
            {
                bpp = t_bpp;
                format_index = i;
                r = t_r;
                g = t_g;
                b = t_b;
                a = t_a;
            }
        }
    }

    // if our mask params are somehow invalid, Surface ctor will throw
    auto size = this->Size();
    auto ws = RDGE::Surface(size.w, size.h, bpp, r, g, b, a);

    // copy the pixels from the renderer to our surface
    if (UNLIKELY(SDL_RenderReadPixels(
                                      m_renderer,
                                      nullptr,  // clip rect
                                      info.texture_formats[format_index],
                                      ws.RawPtr()->pixels,
                                      ws.RawPtr()->pitch
                                     ) != 0))
    {
        SDL_THROW("Failed to read pixels from renderer", "SDL_RenderReadPixels");
    }

    return ws;
}
*/
double
Window::FrameRate (void) const
{
    if (s_frameTimer.IsRunning() == false)
    {
        // make sure all values are set to zero before starting
        std::fill(s_tickSamples, s_tickSamples + FRAME_SAMPLES, 0);
        s_frameTimer.Start();
    }

    // calculate moving average
    auto new_tick = s_frameTimer.TickDelta();
    s_tickSum -= s_tickSamples[s_tickIndex];
    s_tickSum += new_tick;
    s_tickSamples[s_tickIndex] = new_tick;
    if (++s_tickIndex == FRAME_SAMPLES)
    {
        s_tickIndex = 0;
    }

    return 1000.0f / (static_cast<double>(s_tickSum) / static_cast<double>(FRAME_SAMPLES));
}

} // namespace RDGE
