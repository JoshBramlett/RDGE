#include <rdge/glwindow.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <GL/glew.h>

#include <algorithm>

namespace RDGE {

/* TODO: Take this as note:
 *
 * from: https://wiki.libsdl.org/SDL_WindowFlags
 *
 * "On Apple's OS X you must set the NSHighResolutionCapable Info.plist
 * property to YES, otherwise you will not receive a High DPI OpenGL canvas."
 */

namespace {
    GLWindow* s_currentWindow = nullptr;

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

    // Returns a supported aspect ratio if detected, otherwise it'll return an
    // empty size.  Allows for approximation up to one-tenth of a decimal place.
    RDGE::Graphics::Size
    CalculateAspectRatio (RDGE::UInt32 width, RDGE::UInt32 height)
    {
        double epsilon      = 0.01;
        double aspect_ratio = static_cast<double>(width) / static_cast<double>(height);

        // 5 / 4 == 1.25
        if ((aspect_ratio - 1.25) < epsilon)
        {
            return RDGE::Graphics::Size(5, 4);
        }
        // 4 / 3 == 1.33
        else if ((aspect_ratio - 1.33) < epsilon)
        {
            return RDGE::Graphics::Size(4, 3);
        }
        // 16 / 10 == 1.60
        else if ((aspect_ratio - 1.60) < epsilon)
        {
            return RDGE::Graphics::Size(16, 10);
        }
        // 16 / 9 == 1.78
        else if ((aspect_ratio - 1.78) < epsilon)
        {
            return RDGE::Graphics::Size(16, 9);
        }

        return RDGE::Graphics::Size();
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
                auto w = reinterpret_cast<GLWindow*>(user_data);
                w->ResetViewport();
            }

            // TODO: When multiple windows are supported, s_currentWindow must be set
            //       when a window receives focus
        }

        return 1;
    }

} // anonymouse namespace

GLWindow::GLWindow (
                    const std::string& title,
                    RDGE::UInt32       target_width,
                    RDGE::UInt32       target_height,
                    bool               fullscreen,
                    bool               resizable,
                    bool               use_vsync,
                    RDGE::Int32        gl_version_major,
                    RDGE::Int32        gl_version_minor
                   )
    : m_backgroundColor(RDGE::Color::Black())
    , m_targetWidth(target_width)
    , m_targetHeight(target_height)
{
    ILOG("Initializing GLWindow...");

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
    if (
        gl_version_major < MIN_GL_CONTEXT_MAJOR ||
        (gl_version_major == MIN_GL_CONTEXT_MAJOR && gl_version_minor < MIN_GL_CONTEXT_MINOR)
       )
    {
        RDGE_THROW("Requesting unsupported OpenGL version");
    }

    // Ensure a supported aspect ratio can be determined from the target width/height
    m_targetAspectRatio = CalculateAspectRatio(target_width, target_height);
    if (m_targetAspectRatio.IsEmpty())
    {
        RDGE_THROW("Supported aspect ratio cannot be determined from the width/height");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_version_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_version_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    RDGE::UInt32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
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

    RDGE::Int32 interval = use_vsync ? 1 : 0;
    if (UNLIKELY(SDL_GL_SetSwapInterval(interval) != 0))
    {
        auto msg = "SDL failed to set interval VSYNC=" + std::to_string(interval);
        SDL_THROW(msg, "SDL_GL_SetSwapInterval");
    }

    ResetViewport();

    // Self-subscribe to all window events
    SDL_AddEventWatch(&OnWindowEvent, this);

    s_currentWindow = this;
}

GLWindow::~GLWindow (void)
{
    ILOG("Destroying GLWindow...");

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

GLWindow::GLWindow (GLWindow&& rhs) noexcept
    : m_window(rhs.m_window)
    , m_context(rhs.m_context)
    , m_viewport(rhs.m_viewport)
    , m_backgroundColor(rhs.m_backgroundColor)
    , m_targetAspectRatio(rhs.m_targetAspectRatio)
{
    rhs.m_context = nullptr;
    rhs.m_window = nullptr;
}

GLWindow&
GLWindow::operator= (GLWindow&& rhs) noexcept
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
        m_backgroundColor = rhs.m_backgroundColor;
        m_targetAspectRatio = rhs.m_targetAspectRatio;

        rhs.m_context = nullptr;
        rhs.m_window = nullptr;
    }

    return *this;
}

std::string
GLWindow::Title (void) const
{
    return SDL_GetWindowTitle(m_window);
}

RDGE::Graphics::Size
GLWindow::Size (void) const
{
    RDGE::Int32 width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    return RDGE::Graphics::Size(width, height);
}

RDGE::Graphics::Size
GLWindow::DrawableSize (void) const
{
    RDGE::Int32 width, height;
    SDL_GL_GetDrawableSize(m_window, &width, &height);

    return RDGE::Graphics::Size(width, height);
}

RDGE::UInt32
GLWindow::Width (void) const
{
    RDGE::Int32 width;
    SDL_GetWindowSize(m_window, &width, nullptr);

    return static_cast<RDGE::UInt32>(width);
}

RDGE::UInt32
GLWindow::Height (void) const
{
    RDGE::Int32 height;
    SDL_GetWindowSize(m_window, nullptr, &height);

    return static_cast<RDGE::UInt32>(height);
}

void
GLWindow::SetTitle (const std::string& title)
{
    SDL_SetWindowTitle(m_window, title.c_str());
}

void
GLWindow::SetSize (RDGE::UInt32 width, RDGE::UInt32 height)
{
    SDL_SetWindowSize(m_window, width, height);
}

void
GLWindow::SetBackgroundColor (const RDGE::Color& color)
{
    m_backgroundColor = color;
}

void
GLWindow::ResetViewport (void)
{
    RDGE::Int32 draw_width, draw_height;
    SDL_GL_GetDrawableSize(m_window, &draw_width, &draw_height);

    auto target_ratio = m_targetWidth / static_cast<float>(m_targetHeight);
    auto current_ratio = draw_width / static_cast<float>(draw_height);

    if (current_ratio < target_ratio)
    {
        // Will display upper and lower letterbox
        auto sorted = std::minmax(m_targetWidth, static_cast<RDGE::UInt32>(draw_width));
        auto width_ratio = sorted.second / static_cast<float>(sorted.first);

        m_viewport.w = static_cast<RDGE::UInt32>(draw_width);
        m_viewport.h = static_cast<RDGE::UInt32>(m_targetHeight * width_ratio);
        m_viewport.x = 0;
        m_viewport.y = static_cast<RDGE::Int32>((draw_height - m_viewport.h) / 2);
    }
    else
    {
        // Will display left and right letterbox
        auto sorted = std::minmax(m_targetHeight, static_cast<RDGE::UInt32>(draw_height));
        auto height_ratio = sorted.second / static_cast<float>(sorted.first);

        m_viewport.w = static_cast<RDGE::UInt32>(m_targetWidth * height_ratio);
        m_viewport.h = static_cast<RDGE::UInt32>(draw_height);
        m_viewport.x = static_cast<RDGE::Int32>((draw_width - m_viewport.w) / 2);
        m_viewport.y = 0;
    }

    // TODO: Remove
    //std::cout << "draw_width=" << draw_width << std::endl
              //<< "draw_height=" << draw_height << std::endl
              //<< "viewport.x=" << m_viewport.x << std::endl
              //<< "viewport.y=" << m_viewport.y << std::endl
              //<< "viewport.w=" << m_viewport.w << std::endl
              //<< "viewport.h=" << m_viewport.h << std::endl;
}

//void
//GLWindow::PrintViewport (void)
//{
    // TODO: Leaving this here as a reminder to update the glGetIntegerv wrappers
    //RDGE::Int32 testa[2];
    //RDGE::Int32 testb[4];

    //glGetIntegerv(GL_MAX_VIEWPORT_DIMS, testa);
    //glGetIntegerv(GL_VIEWPORT, testb);

    //std::cout << "dims a=" << testa[0] << " b=" << testa[1] << std::endl;
    //std::cout << "vp a=" << testb[0] << " b=" << testb[1]
              //<< " c=" << testb[2] << " d=" << testb[3] << std::endl;
//}

void
GLWindow::Clear (void)
{
    RDGE::Graphics::OpenGL::SetViewport(m_viewport.x, m_viewport.y, m_viewport.w, m_viewport.h);

    // TODO: Values need to be set from the m_clearColor once fp is supported [00032]
    RDGE::Graphics::OpenGL::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    RDGE::Graphics::OpenGL::Clear(GL_COLOR_BUFFER_BIT);
}

void
GLWindow::Present (void)
{
    SDL_GL_SwapWindow(m_window);
}

/*
RDGE::Surface
GLWindow::Screenshot (void)
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
GLWindow::FrameRate (void) const
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

/* static */ const GLWindow*
GLWindow::GetCurrentWindow (void)
{
    return s_currentWindow;
}

} // namespace RDGE
