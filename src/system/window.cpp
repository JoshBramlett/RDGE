#include <rdge/system/window.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/hints.hpp>

#include <GL/glew.h>

#include <algorithm>

using namespace rdge;

/* TODO: Take this as note:
 *
 * from: https://wiki.libsdl.org/SDL_WindowFlags
 *
 * "On Apple's OS X you must set the NSHighResolutionCapable Info.plist
 * property to YES, otherwise you will not receive a High DPI OpenGL canvas."
 */


namespace {
    Window* s_currentWindow = nullptr;

    /*
     * Frame rate timer
     */

    // Number of frames stored to calculate the moving average
    // TODO Move this to a self contained struct
    constexpr rdge::uint32 FRAME_SAMPLES = 100;

    rdge::util::Timer s_frameTimer;
    rdge::uint32      s_tickIndex;
    rdge::uint32      s_tickSum;
    rdge::uint32      s_tickSamples[FRAME_SAMPLES];

    /*
     * Aspect ratio
     */

    // Returns a supported aspect ratio if detected, otherwise it'll return an
    // empty size.  Allows for approximation up to one-tenth of a decimal place.
    rdge::size
    CalculateAspectRatio (rdge::uint32 width, rdge::uint32 height)
    {
        double epsilon      = 0.01;
        double aspect_ratio = static_cast<double>(width) / static_cast<double>(height);

        // 5 / 4 == 1.25
        if ((aspect_ratio - 1.25) < epsilon)
        {
            return rdge::size(5, 4);
        }
        // 4 / 3 == 1.33
        else if ((aspect_ratio - 1.33) < epsilon)
        {
            return rdge::size(4, 3);
        }
        // 16 / 10 == 1.60
        else if ((aspect_ratio - 1.60) < epsilon)
        {
            return rdge::size(16, 10);
        }
        // 16 / 9 == 1.78
        else if ((aspect_ratio - 1.78) < epsilon)
        {
            return rdge::size(16, 9);
        }

        return rdge::size();
    }

    /*
     * Event Listener
     */

    int
    OnWindowEvent (void* user_data, SDL_Event* event)
    {
        if (event->type == SDL_WINDOWEVENT)
        {
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                auto w = reinterpret_cast<Window*>(user_data);
                w->ResetViewport();
            }

            // TODO: When multiple windows are supported, s_currentWindow must be set
            //       when a window receives focus
        }

        return 1;
    }

} // anonymouse namespace

Window::Window (const app_settings& settings)
    : Window(settings.window_title,
             settings.target_width,
             settings.target_height,
             settings.fullscreen,
             settings.resizable,
             settings.use_vsync)
{ }

Window::Window (
                    const std::string& title,
                    rdge::uint32       target_width,
                    rdge::uint32       target_height,
                    bool               fullscreen,
                    bool               resizable,
                    bool               use_vsync,
                    rdge::int32        gl_version_major,
                    rdge::int32        gl_version_minor
                   )
    : m_clearColor(rdge::math::vec4(0.0f, 0.0f, 0.0f, 0.0f))
    , m_targetWidth(target_width)
    , m_targetHeight(target_height)
{
    ILOG("Initializing Window");

    // Ensure video subsystem is created.  Events subsystem (which is required by the
    // class) is initialized automatically by initializing video
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
        if (UNLIKELY(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0))
        {
            SDL_THROW("SDL failed to initialize video subsystem", "SDL_InitSubSystem");
        }
    }

    // Ensure the reuqested context version is not less than our supported version
    //
    // TODO:  I don't think I need to request the OpenGL version to load.  I still
    //        require a minimum version the engine requires for support, but requesting
    //        a lower version than what the graphics card has will load the version
    //        on the card, not the version I'm requesting.  Look into if this is correct.
    if (
        gl_version_major < MIN_GL_CONTEXT_MAJOR ||
        (gl_version_major == MIN_GL_CONTEXT_MAJOR && gl_version_minor < MIN_GL_CONTEXT_MINOR)
       )
    {
        RDGE_THROW("Requesting unsupported OpenGL version");
    }

    // Ensure a supported aspect ratio can be determined from the target width/height
    m_targetAspectRatio = CalculateAspectRatio(target_width, target_height);
    // TODO: I don't like IsEmpty (check if zero) for the point/size type since it's valid
    //       figure out a better way to implement this
    //if (m_targetAspectRatio.IsEmpty())
    //{
        //RDGE_THROW("Supported aspect ratio cannot be determined from the width/height");
    //}

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_version_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_version_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // TODO: This should be a config setting.  This value must be set before creating
    //       the context, so you cannot query whether the amount exceeds the max.
    //       One option would be to get the actual value via SDL_GL_GetAttribute, or
    //       from OpenGL::GetIntegerValue(GL_SAMPLES) with GL_MAX_SAMPLES to get the
    //       value max allowed and log that to a file.  OpenGL requires this value be
    //       a minimum of 4.
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    rdge::uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (resizable)
    {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    m_window = SDL_CreateWindow(
                                title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                target_width,
                                target_height,
                                flags
                               );
    if (UNLIKELY(!m_window))
    {
        SDL_THROW("SDL failed to create a Window", "SDL_CreateWindow");
    }

    m_context = SDL_GL_CreateContext(m_window);
    if (UNLIKELY(!m_context))
    {
        SDL_THROW("SDL failed to create an OpenGL context", "SDL_GL_CreateContext");
    }

    ILOG("Created OpenGL context v" + opengl::GetStringValue(GL_VERSION));

    // !!!  IMPORTANT  !!!
    // There are a couple issues with GLEW initialization (as far as 1.13).
    //     1) GLEW has a problem with OpenGL 3.2+ core contexts, and the only fix
    //        is to set glewExperimental to TRUE.
    //     2) You may get GL_INVALID_ENUM when initializing.  Everything *should*
    //        work, but a query to glGetError is required to further suppress
    //        the error going forward.
    //
    // http://stackoverflow.com/questions/10857335/opengl-glgeterror-returns-invalid-enum-after-call-to-glewinit
    // https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW_.28OpenGL_Extension_Wrangler.29

    // Must be set to true for OpenGL 3.2+ contexts
    glewExperimental = GL_TRUE;
    GLenum glew_error = glewInit();
    if (UNLIKELY(glew_error != GLEW_OK))
    {
        std::string err_msg = reinterpret_cast<const char*>(glewGetErrorString(glew_error));
        RDGE_THROW("Failed to initialize GLEW.  error=" + err_msg);
    }

    // Query for the OpenGL error to clear the value for later lookup
    glGetError();

    // TODO:  This enables alpha blending.  Look into it further when I have time.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    rdge::int32 interval = use_vsync ? 1 : 0;
    if (UNLIKELY(SDL_GL_SetSwapInterval(interval) != 0))
    {
        // TODO: This should log a warning and default to not using vsync.  Fallback
        //       must be implemented if vsync is unable to be loaded.
        auto msg = "SDL failed to set interval VSYNC=" + std::to_string(interval);
        SDL_THROW(msg, "SDL_GL_SetSwapInterval");
    }

    if (use_vsync)
    {
        ILOG("Swap interval set to use VSYNC");
    }

    ResetViewport();

    // Self-subscribe to all window events
    SDL_AddEventWatch(&OnWindowEvent, this);

    s_currentWindow = this;
}

Window::~Window (void)
{
    ILOG("Destroying Window");

    SDL_DelEventWatch(&OnWindowEvent, this);

    if (m_context != nullptr)
    {
        SDL_GL_DeleteContext(m_context);
    }

    if (m_window != nullptr)
    {
        SDL_DestroyWindow(m_window);
    }
}

Window::Window (Window&& rhs) noexcept
    : m_window(rhs.m_window)
    , m_context(rhs.m_context)
    , m_viewport(rhs.m_viewport)
    , m_clearColor(rhs.m_clearColor)
    , m_targetAspectRatio(rhs.m_targetAspectRatio)
{
    rhs.m_context = nullptr;
    rhs.m_window = nullptr;
}

Window&
Window::operator= (Window&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_context != nullptr)
        {
            SDL_GL_DeleteContext(m_context);
        }

        if (m_window != nullptr)
        {
            SDL_DestroyWindow(m_window);
        }

        m_context = rhs.m_context;
        m_window = rhs.m_window;
        m_viewport = rhs.m_viewport;
        m_clearColor = rhs.m_clearColor;
        m_targetAspectRatio = rhs.m_targetAspectRatio;

        rhs.m_context = nullptr;
        rhs.m_window = nullptr;
    }

    return *this;
}

std::string
Window::Title (void) const
{
    return SDL_GetWindowTitle(m_window);
}

rdge::size
Window::Size (void) const
{
    rdge::int32 width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    return rdge::size(width, height);
}

rdge::size
Window::DrawableSize (void) const
{
    rdge::int32 width, height;
    SDL_GL_GetDrawableSize(m_window, &width, &height);

    return rdge::size(width, height);
}

rdge::uint32
Window::Width (void) const
{
    rdge::int32 width;
    SDL_GetWindowSize(m_window, &width, nullptr);

    return static_cast<rdge::uint32>(width);
}

rdge::uint32
Window::Height (void) const
{
    rdge::int32 height;
    SDL_GetWindowSize(m_window, nullptr, &height);

    return static_cast<rdge::uint32>(height);
}

void
Window::SetTitle (const std::string& title)
{
    DLOG("Setting window title to " + title);

    SDL_SetWindowTitle(m_window, title.c_str());
}

void
Window::SetSize (rdge::uint32 width, rdge::uint32 height)
{
    DLOG("Setting window size to [" + std::to_string(width) + "," + std::to_string(height) + "]");

    SDL_SetWindowSize(m_window, width, height);
}

void
Window::SetCursorLocation (const rdge::point& location)
{
    SDL_WarpMouseInWindow(m_window, location.x, location.y);
}

void
Window::SetClearColor (const rdge::color& color)
{
    m_clearColor = static_cast<rdge::math::vec4>(color);
}

void
Window::ResetViewport (void)
{
    // TODO This is where our letter box is set.  This should stay, but be refactored
    //      to become more extensible.  Aspect ratio is the core, but rendering outside
    //      of that could be left to the implementation.  For example, instead of a
    //      letterbox the background could be extended (but the main game stays within
    //      the bounds of the aspect ratio)
    rdge::int32 draw_width, draw_height;
    SDL_GL_GetDrawableSize(m_window, &draw_width, &draw_height);

    auto target_ratio = m_targetWidth / static_cast<float>(m_targetHeight);
    auto current_ratio = draw_width / static_cast<float>(draw_height);

    if (current_ratio < target_ratio)
    {
        // Will display upper and lower letterbox
        auto sorted = std::minmax(m_targetWidth, static_cast<rdge::uint32>(draw_width));
        auto width_ratio = sorted.second / static_cast<float>(sorted.first);

        m_viewport.w = static_cast<rdge::uint32>(draw_width);
        m_viewport.h = static_cast<rdge::uint32>(m_targetHeight * width_ratio);
        m_viewport.x = 0;
        m_viewport.y = static_cast<rdge::int32>((draw_height - m_viewport.h) / 2);
    }
    else
    {
        // Will display left and right letterbox
        auto sorted = std::minmax(m_targetHeight, static_cast<rdge::uint32>(draw_height));
        auto height_ratio = sorted.second / static_cast<float>(sorted.first);

        m_viewport.w = static_cast<rdge::uint32>(m_targetWidth * height_ratio);
        m_viewport.h = static_cast<rdge::uint32>(draw_height);
        m_viewport.x = static_cast<rdge::int32>((draw_width - m_viewport.w) / 2);
        m_viewport.y = 0;
    }
}

void
Window::Clear (void)
{
    opengl::SetViewport(m_viewport.x, m_viewport.y, m_viewport.w, m_viewport.h);
#ifdef RDGE_DEBUG
    // Set clear color to gray during debug to help determine if OpenGL is rendering
    // correctly.  Note, this will make SetClearColor appear to not work.
    opengl::SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);;
#else
    opengl::SetClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
#endif

    opengl::Clear(GL_COLOR_BUFFER_BIT);
}

void
Window::Present (void)
{
    SDL_GL_SwapWindow(m_window);
}

/*
RDGE::Surface
Window::Screenshot (void)
{
    // TODO: Implement
    // taken from http://stackoverflow.com/questions/20233469
    unsigned char * pixels = new unsigned char[w*h*4]; // 4 bytes for RGBA
    glReadPixels(x,y,w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels, w, h, 8*4, w*4, 0,0,0,0);
    SDL_SaveBMP(surf, filename);

    SDL_FreeSurface(surf);
    delete [] pixels;
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

/* static */ const Window*
Window::GetCurrentWindow (void)
{
    return s_currentWindow;
}
