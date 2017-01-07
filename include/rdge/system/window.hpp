//! \headerfile <rdge/system/window.hpp>
//! \author Josh Bramlett
//! \version 0.0.9
//! \date 06/14/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/assets/surface.hpp>

#include <SDL.h>

#include <string>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

struct aspect_ratio
{
    float ratio;
    uint32 default_width;
    uint32 default_height;
    uint8 x;
    uint8 y;
};

//enum class AspectRatio : uint8
//{
    //5x4,
    //4x3,
    //16z10,
    //16x9
//};

//! \struct viewport
//! \brief Rendering viewport (subscreen)
//! \details Represents the viewport set by glViewport, which are the drawable
//!          window coordinates.  These values may be larger than the window size
//!          when rendering to a platform where high DPI is supported.
struct viewport
{
    //! \var x Lower left x-coordinate of the drawing rectangle
    rdge::int32  x;
    //! \var y Lower left y-coordinate of the drawing rectangle
    rdge::int32  y;
    //! \var width Width of the viewport
    rdge::uint32 w;
    //! \var height Height of the viewport
    rdge::uint32 h;
};

//! \class Window
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
class Window
{
public:
    // TODO document
    explicit Window (const app_settings& settings);

    //! \brief Window ctor
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
    explicit Window (
                       const std::string& title,
                       rdge::uint32       target_width,
                       rdge::uint32       target_height,
                       bool               fullscreen       = false,
                       bool               resizable        = false,
                       bool               use_vsync        = false
                      );

    //! \brief Window dtor
    //! \details Destroy SDL objects
    ~Window (void);

    //! \brief Window Copy ctor
    //! \details Non-copyable
    Window (const Window&) = delete;

    //! \brief Window Move ctor
    //! \details Transfers ownership
    Window (Window&&) noexcept;

    //! \brief Window Copy Assignment Operator
    //! \details Non-copyable
    Window& operator= (const Window&) = delete;

    //! \brief Window Move Assignment Operator
    //! \details Transfers ownership
    Window& operator= (Window&&) noexcept;

    //! \brief Get the window title
    //! \return Title of the window
    std::string Title (void) const;

    //! \brief Get the window size
    //! \return Size structure
    math::uivec2 Size (void) const;

    //! \brief Get the window's drawable size
    //! \details The drawable size can differ from the window size for platforms
    //!          which have high DPI support.
    //! \return Size structure
    math::uivec2 DrawableSize (void) const;

    //! \brief Get the window width
    //! \return Width of the window
    uint32 Width (void) const;

    //! \brief Get the window height
    //! \return Height of the window
    uint32 Height (void) const;

    //! \brief Get the target window width
    //! \return Target drawing width
    uint32 TargetWidth (void) const { return m_targetWidth; }

    //! \brief Get the target window height
    //! \return Target drawing height
    uint32 TargetHeight (void) const { return m_targetHeight; }

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
    void SetSize (uint32 width, uint32 height);

    //! \brief Set the cursor location
    //! \param [in] location Point containing the x and y coordinates
    void SetCursorLocation (const point& location);

    //! \brief Set the background color
    //! \details Color presented to the screen before any drawing takes place
    //! \param [in] color Color structure
    void SetClearColor (const rdge::color& color);

    //! \brief Calculates and sets the window's drawing viewport
    //! \details This should never be called directly.  It is used by the
    //!          Window when listening to window events.
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
    static const Window* GetCurrentWindow (void);

private:
    SDL_Window*   m_window;
    SDL_GLContext m_context;

    rdge::viewport   m_viewport;
    rdge::math::vec4 m_clearColor;

    rdge::uint32    m_targetWidth;
    rdge::uint32    m_targetHeight;
};

} // namespace rdge
