//! \headerfile <rdge/glwindow.hpp>
//! \author Josh Bramlett
//! \version 0.0.4
//! \date 05/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/color.hpp>
#include <rdge/graphics/size.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/assets/surface.hpp>

#include <SDL.h>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \struct viewport
//! \brief Rendering viewport (subscreen)
//! \details Represents the viewport set by glViewport, which are the drawable
//!          window coordinates.  These values may be larger than the window size
//!          when rendering to a platform where high DPI is supported.
struct viewport
{
    //! \var x Lower left x-coordinate of the drawing rectangle
    RDGE::Int32  x;
    //! \var y Lower left y-coordinate of the drawing rectangle
    RDGE::Int32  y;
    //! \var width Width of the viewport
    RDGE::UInt32 w;
    //! \var height Height of the viewport
    RDGE::UInt32 h;
};

//! \class GLWindow
//! \brief Window and OpenGL context management
//! \details Represents a window to draw to.  The class is responsible for
//!          creating and managing the SDL_Window and OpenGL context.  The viewport
//!          is generated automatically depending on the target width/height of
//!          the window.  Whenever the screen is resized the viewport will be
//!          recalculated to provide a letterbox effect.
//! \note The context profile created is the core profile.  Therefore
//!       deprecated functions are disabled, and since the engine is
//!       intended for desktop applications, an option to create an
//!       OpenGL ES context is not available.
class GLWindow
{
public:
    // TODO: The OpenGL context version (params below) will load the highest
    //       compatible version with the requested version.  So requesting
    //       version 3.3 could still load 4.1.  I need to determine the lowest
    //       version supported within the RDGE library, and disallow any request
    //       below that version.

    //! \var MIN_GL_CONTEXT_MAJOR Minumum supported OpenGL context major version
    static constexpr RDGE::Int32 MIN_GL_CONTEXT_MAJOR = 3;
    //! \var MIN_GL_CONTEXT_MINOR Minumum supported OpenGL context minor version
    static constexpr RDGE::Int32 MIN_GL_CONTEXT_MINOR = 3;

    //! \brief GLWindow ctor
    //! \details Initialize SDL window and renderer
    //! \param [in] title Window title
    //! \param [in] target_width Window width
    //! \param [in] target_height Window height
    //! \param [in] fullscreen Window is created in fullscreen mode
    //! \param [in] resizable Window is resizable
    //! \param [in] use_vsync Set renderer to use VSYNC
    //! \param [in] gl_version_major OpenGL context major version to use
    //! \param [in] gl_version_minor OpenGL context minor version to use
    //! \throws RDGE::Exception Unsupported context version
    //! \throws RDGE::SDLException Initialization failed
    explicit GLWindow (
                       const std::string& title,
                       RDGE::UInt32       target_width,
                       RDGE::UInt32       target_height,
                       bool               fullscreen       = false,
                       bool               resizable        = false,
                       bool               use_vsync        = false,
                       RDGE::Int32        gl_version_major = MIN_GL_CONTEXT_MAJOR,
                       RDGE::Int32        gl_version_minor = MIN_GL_CONTEXT_MINOR
                      );

    //! \brief GLWindow dtor
    //! \details Destroy SDL objects
    ~GLWindow (void);

    //! \brief GLWindow Copy ctor
    //! \details Non-copyable
    GLWindow (const GLWindow&) = delete;

    //! \brief GLWindow Move ctor
    //! \details Transfers ownership
    GLWindow (GLWindow&&) noexcept;

    //! \brief GLWindow Copy Assignment Operator
    //! \details Non-copyable
    GLWindow& operator= (const GLWindow&) = delete;

    //! \brief GLWindow Move Assignment Operator
    //! \details Transfers ownership
    GLWindow& operator= (GLWindow&&) noexcept;

    //! \brief Get the window title
    //! \return Title of the window
    std::string Title (void) const;

    //! \brief Get the window size
    //! \return Size structure
    RDGE::Graphics::Size Size (void) const;

    //! \brief Get the window's drawable size
    //! \details The drawable size can differ from the window size for platforms
    //!          which have high DPI support.
    //! \return Size structure
    RDGE::Graphics::Size DrawableSize (void) const;

    //! \brief Get the window width
    //! \return Width of the window
    RDGE::UInt32 Width (void) const;

    //! \brief Get the window height
    //! \return Height of the window
    RDGE::UInt32 Height (void) const;

    //! \brief Get the target window width
    //! \return Target drawing width
    RDGE::UInt32 TargetWidth (void) const { return m_targetWidth; }

    //! \brief Get the target window height
    //! \return Target drawing height
    RDGE::UInt32 TargetHeight (void) const { return m_targetHeight; }

    //! \brief Get the window's target aspect ratio
    //! \return Size representing the aspect ratio
    //TODO Return ivec2 when available
    const RDGE::Graphics::Size& TargetAspectRatio (void) const { return m_targetAspectRatio; }

    //! \brief Return the SDL_Window pointer
    //! \details Raw pointer is returned so caller must ensure
    //!          Window object will not fall out of scope
    //! \return Raw pointer to an SDL Window
    SDL_Window* RawPtr (void) const { return m_window; }

    //! \brief Set the window title
    //! \param [in] title Title of the window
    void SetTitle (const std::string& title);

    //! \brief Set the window size
    //! \param [in] width Width in pixels
    //! \param [in] height Height in pixels
    void SetSize (RDGE::UInt32 width, RDGE::UInt32 height);

    //! \brief Set the background color
    //! \details Color presented to the screen before any drawing takes place
    //! \param [in] color Color structure
    void SetClearColor (const RDGE::Color& color);

    //! \brief Calculates and sets the window's drawing viewport
    //! \details This should never be called directly.  It is used by the
    //!          GLWindow when listening to window events.
    void ResetViewport (void);

    //! \brief Clear the window to prepare for drawing
    void Clear (void);

    //! \brief Present the renderer buffer to the screen
    void Present (void);

    //! \brief Get a screenshot of the window
    //! \details Saves the pixel data from the currently rendered screen
    //!          in an RGB \ref Surface.  This method is slow, so it should
    //!          be used sparingly (i.e. not within the game loop).
    //! \note Result can be passed to SDL_SaveBMP to save to file
    //! \returns Surface screenshot
    //RDGE::Surface Screenshot (void);

    //! \brief Get the avarge frame rate
    //! \details It's imperative this be called every frame or the values
    //!          returned will not be accurate.  The frame rate is
    //!          calculated using a simple moving average of the duration
    //!          between when this method is called.  All values reported
    //!          until the threshold set in MAX_FRAME_SAMPLES has been hit
    //!          can be discarded.
    //! \note The calculations are self contained so not calling this method
    //!       has no performance impact elsewhere in the class
    //! \returns Frame rate
    double FrameRate (void) const;

    //! \brief Get a pointer to the current window
    //! \details The current window is that which is in focus.
    //! \returns Pointer to window (or nullptr if not available)
    static const GLWindow* GetCurrentWindow (void);

private:
    SDL_Window*   m_window;
    SDL_GLContext m_context;

    RDGE::viewport   m_viewport;
    RDGE::Math::vec4 m_clearColor;

    RDGE::UInt32         m_targetWidth;
    RDGE::UInt32         m_targetHeight;
    RDGE::Graphics::Size m_targetAspectRatio;
};

} // namespace RDGE
