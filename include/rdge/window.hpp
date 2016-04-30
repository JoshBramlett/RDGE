//! \headerfile <rdge/window.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 10/25/2015
//! \bug

#pragma once

#include <string>
#include <memory>

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/texture.hpp>
#include <rdge/graphics/rect.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \typedef SDLWindowUniquePtr
//! \details Proper unique_ptr type implementing SDL custom deleter
//! \note This wraps the native SDL_Window, not the RDGE::Window.  It's
//!       available as an alternative to the RAII object
using SDLWindowUniquePtr = std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*>;

//! \typedef SDLRendererUniquePtr
//! \details Proper unique_ptr type implementing SDL custom deleter
//! \note This wraps the native SDL_Renderer
using SDLRendererUniquePtr = std::unique_ptr<SDL_Renderer, decltype(SDL_DestroyRenderer)*>;

//! \brief Create unique_ptr wrapper for native SDL_Window
//! \details Helper function to allocate and wrap an SDL_Window in a
//!          unique_ptr with the proper custom deleter
//! \param [in] title Window title
//! \param [in] x x-coordinate placement
//! \param [in] y y-coordinate placement
//! \param [in] w Window width
//! \param [in] h Window height
//! \param [in] flags SDL_WindowFlags bitset
//! \returns Allocated SDL_Window wrapped in a unique_ptr
inline auto CreateSDLWindowUniquePtr (
                                      const char* title,
                                      RDGE::Int32 x, RDGE::Int32 y,
                                      RDGE::Int32 w, RDGE::Int32 h,
                                      RDGE::UInt32 flags
                                     ) -> SDLWindowUniquePtr
{
    return SDLWindowUniquePtr(SDL_CreateWindow(title, x, y, w, h, flags), SDL_DestroyWindow);
}

//! \brief Create unique_ptr wrapper for native SDL_Renderer
//! \details Helper function to allocate and wrap an SDL_Renderer in a
//!          unique_ptr with the proper custom deleter
//! \param [in] window SDL_Window pointer
//! \param [in] index Index of the rendering driver
//! \param [in] flags SDL_RendererFlags bitset
//! \returns Allocated SDL_Renderer wrapped in a unique_ptr
inline auto CreateRendererUniquePtr (
                                     SDL_Window*  window,
                                     RDGE::Int32  index,
                                     RDGE::UInt32 flags
                                    ) -> SDLRendererUniquePtr
{
    return SDLRendererUniquePtr(SDL_CreateRenderer(window, index, flags), SDL_DestroyRenderer);
}

//! \enum AspectRatio
//! \details List of supported aspect ratios
enum class AspectRatio : RDGE::UInt8
{
    //! \brief Invalid aspect ratio
    Invalid = 0,
    //! \brief 5:4
    R5_4    = 1,
    //! \brief 4:3
    R4_3    = 2,
    //! \brief 16:10
    R16_10  = 3,
    //! \brief 16:9
    R16_9   = 4
};

//! \class Window
//! \brief Window and renderer management
//! \details Wraps functionality from both the SDL_Window and SDL_Renderer
//!          in an RAII compliant object.  Provides functionality to draw
//!          textures to the screen.
//! \note Window maintains exclusive ownership of the native SDL_Window and
//!       SDL_Renderer objects, therefore creating copies is prohibited.
//!       Implementation opts to store raw pointers over unique_ptrs as
//!       smart pointers are surpufluous with the limited scope
class Window
{
public:
    //! \brief Default Window ctor
    Window (void);

    //! \brief Window ctor
    //! \details Initialize SDL window and renderer
    //! \param [in] title Window title
    //! \param [in] width Window width
    //! \param [in] height Window height
    //! \param [in] use_vsync Set renderer to use VSYNC
    //! \throws Initialization failed
    explicit Window (
                     const std::string& title,
                     RDGE::UInt32       width,
                     RDGE::UInt32       height,
                     bool               use_vsync = false
                    );

    //! \brief Window dtor
    //! \details Destroy SDL objects
    ~Window (void);

    //! \brief Window Copy ctor
    //! \details Non-copyable
    Window (const Window&) = delete;

    //! \brief Window Move ctor
    //! \details Transfers ownership of raw pointers
    Window (Window&& rhs) noexcept;

    //! \brief Window Copy Assignment Operator
    //! \details Non-copyable
    Window& operator= (const Window&) = delete;

    //! \brief Window Move Assignment Operator
    //! \details Transfers ownership of raw pointers
    Window& operator= (Window&& rhs) noexcept;

    //! \brief Get the window title
    //! \return Title of the window
    std::string Title (void) const;

    //! \brief Get the window size
    //! \return Size structure
    RDGE::Graphics::Size Size (void) const;

    //! \brief Get the window width
    //! \return Width of the window
    RDGE::UInt32 Width (void) const;

    //! \brief Get the window height
    //! \return Height of the window
    RDGE::UInt32 Height (void) const;

    //! \brief Get the background color
    //! \return Color structure
    RDGE::Color BackgroundColor (void) const { return m_backgroundColor; }

    //! \brief Get the aspect ratio
    //! \return Aspect ratio
    enum AspectRatio AspectRatio (void) const { return m_aspectRatio; }

    //! \brief Return the SDL_Renderer pointer
    //! \details Raw pointer is returned so caller must ensure
    //!          Window object will not fall out of scope
    //! \return Raw pointer to an SDL Renderer
    SDL_Renderer* Renderer (void) const { return m_renderer; }

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

    //! \brief Set the window size
    //! \param [in] size Size structure denoting width and height
    void SetSize (const RDGE::Graphics::Size& size);

    //! \brief Set the background color
    //! \details Color presented to the screen before any drawing takes place
    //! \param [in] color Color structure
    void SetBackgroundColor (const RDGE::Color& color);

    //! \brief Clear the window to prepare for drawing
    void Clear (void);

    //! \brief Present the renderer buffer to the screen
    void Present (void);

    //! \brief Draw texture to the renderer
    //! \details Draw full texture to the renderer at the specified point
    //! \param [in] texture SDL_Texture to draw
    //! \param [in] point Destination location coordinates
    //! \param [in] clip Clip of source to draw
    //! \throws Texture copy to renderer failed
    void Draw (
               const RDGE::Texture&         texture,
               const RDGE::Graphics::Point& point,
               const RDGE::Graphics::Rect&  clip = RDGE::Graphics::Rect::Empty()
              ) const;

    //! \brief Draw texture to the renderer
    //! \details Draw cipped texture to the renderer
    //! \param [in] texture SDL_Texture to draw
    //! \param [in] destination Location on renderer
    //! \param [in] clip Clip of source to draw
    //! \throws Texture copy to renderer failed
    void Draw (
               const RDGE::Texture&        texture,
               const RDGE::Graphics::Rect& destination,
               const RDGE::Graphics::Rect& clip = RDGE::Graphics::Rect::Empty()
              ) const;

    //! \brief Draw rectangle to renderer
    //! \details Draw rectangle without need for a texture.  The default
    //!          behavior is to blend based on the color alpha.
    //! \param [in] rect Pointer to rect structure
    //! \param [in] color Color to draw the rectangle
    //! \param [in] fill Fill with solid color, 1px border if false
    //! \param [in] blend_mode SDL_BlendMode for alpha blending
    //! \throws Failed to render rectangle
    void DrawPrimitiveRect (
                            RDGE::Graphics::Rect* rect,
                            const RDGE::Color&    color,
                            bool                  fill       = true,
                            SDL_BlendMode         blend_mode = SDL_BLENDMODE_BLEND
                           ) const;

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

private:
    RDGE::Color      m_backgroundColor;
    enum AspectRatio m_aspectRatio;

    SDL_Renderer* m_renderer;
    SDL_Window*   m_window;
};

} // namespace RDGE
