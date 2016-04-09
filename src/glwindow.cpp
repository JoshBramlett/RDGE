#include <rdge/glwindow.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>

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
    /*
     * Frame rate timer
     */

    // Number of frames stored to calculate the moving average
    constexpr RDGE::UInt32 FRAME_SAMPLES = 100;

    RDGE::Util::Timer s_frameTimer;
    RDGE::UInt32      s_tickIndex;
    RDGE::UInt32      s_tickSum;
    RDGE::UInt32      s_tickSamples[FRAME_SAMPLES];

    constexpr RDGE::Int32 MinGLContextMajorVersion = 4;
    constexpr RDGE::Int32 MinGLContextMinorVersion = 1;

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
{
    // Ensure the reuqested context version is not less than our supported version
    if (gl_version_major < MinGLContextMajorVersion)
    {
        gl_version_major = MinGLContextMajorVersion;
        gl_version_minor = MinGLContextMinorVersion;
    }
    else if (gl_version_major == MinGLContextMajorVersion)
    {
        if (gl_version_minor < MinGLContextMinorVersion)
        {
            gl_version_minor = MinGLContextMinorVersion;
        }
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_version_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_version_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

    RDGE::Int32 interval = use_vsync ? 1 : 0;
    if (SDL_GL_SetSwapInterval(interval) != 0)
    {
        auto msg = "SDL failed to set interval VSYNC=" + std::to_string(interval);
        SDL_THROW(msg, "SDL_GL_SetSwapInterval");
    }
}

GLWindow::~GLWindow (void)
{
    if (m_window != nullptr)
    {
        SDL_DestroyWindow(m_window);
    }
}

GLWindow::GLWindow (GLWindow&& rhs) noexcept
    : m_window(rhs.m_window)
{
    rhs.m_window = nullptr;
}

GLWindow&
GLWindow::operator= (GLWindow&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (!m_window)
        {
            // this branch should never happen.  In fact it'd be pretty bad if it did.
            // should we throw?  Keep in mind it's flagged noexcept
            // TODO:  Finally look into asserts
            SDL_DestroyWindow(m_window);
        }

        m_window = rhs.m_window;
        rhs.m_window = nullptr;
    }

    return *this;
}

void
GLWindow::Clear (void)
{
    // TODO: Values need to be set from the m_clearColor once fp is supported [00032]
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
GLWindow::Present (void)
{
    SDL_GL_SwapWindow(m_window);
}

RDGE::Surface
GLWindow::Screenshot (void)
{
    // TODO: Implement
    return RDGE::Surface(nullptr);
}

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

} // namespace RDGE
