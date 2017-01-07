#include <rdge/system/window.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/debug/sdl_dumper.hpp>

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

    // TODO Got a bit of work to do with understanding how to properly set up SDL and OpenGL.
    //      Seems to be a good reference:
    //      https://github.com/FNA-XNA/FNA/blob/master/src/FNAPlatform/SDL2_FNAPlatform.cs

namespace {

// Minimum OpenGL context required
constexpr int32 MIN_GL_CONTEXT_MAJOR = 3;
constexpr int32 MIN_GL_CONTEXT_MINOR = 3;


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
                    bool               use_vsync
                   )
    : m_clearColor(math::vec4(0.0f, 0.0f, 0.0f, 0.0f))
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

    // TODO GL context version check was temporarily removed to refactor the interface.
    //      Chould be re-added using the application settings

    // Ensure the reuqested context version is not less than our supported version
    //if (gl_version_major < MIN_GL_CONTEXT_MAJOR ||
        //(gl_version_major == MIN_GL_CONTEXT_MAJOR && gl_version_minor < MIN_GL_CONTEXT_MINOR))
    //{
        //RDGE_THROW("Requesting unsupported OpenGL version");
    //}

    // ****************** SDL OpenGL Attributes ******************
    // All SDL_GLattr values must be set prior to the window and context creation.
    // In most cases the values set are merely hints to OpenGL, so the actual values
    // may differ than the request.  You can query the values after context creation
    // for review.
    //
    // TODO These should be configurable based on the target, but the problem is
    //      the values must be set prior to context creation so there is no way to
    //      query for the values.  Either the values need to be configured for
    //      OpenGL guaranteed minimums, or I need to figure out a way to query
    //      for the values.
    //
    //      *Update* Handmade creates a dummy window to query for the values, then
    //      destroys it and creates the real window.

    // Context version will be equal to or higher than the requested value, or fail
    // the request cannot be met.  The profile mask accepts core, compatibility, or
    // GLES.  SDL docs say that passing zero will let SDL choose, but testing on
    // OSX fails to create anything but the core profile.
    // https://wiki.libsdl.org/SDL_GLprofile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MIN_GL_CONTEXT_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MIN_GL_CONTEXT_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // TODO Handmade goes through srgb, what it is and why it's used.  Make sure to
    //      look it up to understand what it is.  Also, the SDL port to handmade
    //      attempts to create a window with it, and if it fails creates one without it.
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    // Frame buffer
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Color buffer
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Set to 1 to require hardware acceleration, and 0 to force software rendering.
    // Omitting the call allows either.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // The forward compatible flag means that no deprecated functionality will be supported.
    // Can result in a performance gain, and is only for GL 3.0 and later contexts.  Flag is
    // implementation defined, so it seems to do nothing on os x.
    // https://wiki.libsdl.org/SDL_GLcontextFlag
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    // Enable multisampling, which is a type of anti-aliasing.  We create a buffer and set the
    // number of samples per pixel.  Basically the fragment shader is run with vertex data
    // interpolated to the center of the pixel, which stores the color as a sub-sample.  These
    // are averaged in order to determine the final color.
    // http://www.learnopengl.com/#!Advanced-OpenGL/Anti-aliasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (resizable)
    {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    m_window = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                target_width,
                                target_height,
                                flags);
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

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_LEQUAL, 0);

    // TODO:  This enables alpha blending.  Look into it further when I have time.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);

    int32 interval = use_vsync ? 1 : 0;
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

math::uivec2
Window::Size (void) const
{
    int32 width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    return { static_cast<uint32>(width), static_cast<uint32>(height) };
}

math::uivec2
Window::DrawableSize (void) const
{
    int32 width, height;
    SDL_GL_GetDrawableSize(m_window, &width, &height);

    return { static_cast<uint32>(width), static_cast<uint32>(height) };
}

uint32
Window::Width (void) const
{
    int32 width;
    SDL_GetWindowSize(m_window, &width, nullptr);

    return static_cast<rdge::uint32>(width);
}

uint32
Window::Height (void) const
{
    int32 height;
    SDL_GetWindowSize(m_window, nullptr, &height);

    return static_cast<uint32>(height);
}

void
Window::SetTitle (const std::string& title)
{
    DLOG("Setting window title to " + title);

    SDL_SetWindowTitle(m_window, title.c_str());
}

void
Window::SetSize (uint32 width, uint32 height)
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
    int32 draw_width, draw_height;
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

    opengl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
