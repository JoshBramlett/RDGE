//! \headerfile <rdge/glwindow.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/26/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/surface.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \class GLWindow
//! \brief Window and OpenGL context/renderer management
//! \details The base Window class is responsible for the ownership of
//!          the SDL_Window we're drawing to.  The GLWindow will create
//!          the OpenGL context and bind it to the window, and provides
//!          methods for rendering to the window.
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
    //! \throws Initialization failed
    explicit GLWindow (
                       const std::string& title,
                       RDGE::UInt32       target_width,
                       RDGE::UInt32       target_height,
                       bool               fullscreen       = false,
                       bool               resizable        = false,
                       bool               use_vsync        = false,
                       RDGE::Int32        gl_version_major = 0,
                       RDGE::Int32        gl_version_minor = 0
                      );

    //! \brief GLWindow dtor
    //! \details Destroy SDL objects
    ~GLWindow (void);

    //! \brief GLWindow Copy ctor
    //! \details Non-copyable
    GLWindow (const GLWindow&) = delete;

    //! \brief GLWindow Move ctor
    //! \details Transfers ownership of raw pointers
    GLWindow (GLWindow&& rhs) noexcept;

    //! \brief GLWindow Copy Assignment Operator
    //! \details Non-copyable
    GLWindow& operator= (const GLWindow&) = delete;

    //! \brief GLWindow Move Assignment Operator
    //! \details Transfers ownership of raw pointers
    GLWindow& operator= (GLWindow&& rhs) noexcept;

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
    RDGE::Surface Screenshot (void);

private:
    SDL_Window*   m_window;
    SDL_GLContext m_context;
};

} // namespace RDGE
