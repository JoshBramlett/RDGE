//! \headerfile <rdge/surface.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/07/2016
//! \bug

#pragma once

#include <string>
#include <memory>

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/graphics/size.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \typedef SDLSurfaceUniquePtr
//! \details Proper unique_ptr type implementing SDL custom deleter
//! \note This wraps the native SDL_Surface, not the RDGE::Surface.  It's
//!       available as an alternative to the RAII object
using SDLSurfaceUniquePtr = std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>;

//! \brief Create unique_ptr wrapper for native SDL_Surface
//! \details Helper function to wrap an SDL_Surface in a unique_ptr
//!          with the proper custom deleter
//! \param [in] surface Pre-allocated SDL_Surface
//! \returns unique_ptr of provided SDL_Surface
inline auto CreateSDLSurfaceUniquePtr (SDL_Surface* surface) -> SDLSurfaceUniquePtr
{
    return SDLSurfaceUniquePtr(surface, SDL_FreeSurface);
}

//! \class Surface
//! \details RAII complient wrapper for SDL_Surface
//! \brief Surface data is stored in ram and is slower than it's
//!        drawing counterpart \ref Texture.  Surface objects should
//!        not be used for rendering
class Surface
{
public:
    //! \brief Surface ctor
    //! \param [in] surface Pointer to SDL_Surface
    explicit Surface (SDL_Surface* surface);

    //! \brief Surface ctor
    //! \details Uses the SDL_image library to load from file
    //! \param [in] file File path of the image to load
    //! \throws Image initialization failed
    explicit Surface (const std::string& file);

    //! \brief Surface ctor
    //! \details Create a blank RGB surface
    //! \param [in] width Width of the surface
    //! \param [in] height Height of the surface
    //! \param [in] depth Pixel depth (bits per pixel)
    //! \param [in] r_mask Red mask for the pixels
    //! \param [in] g_mask Green mask for the pixels
    //! \param [in] b_mask Blue mask for the pixels
    //! \param [in] a_mask Alpha mask for the pixels
    //! \throws Image initialization failed
    explicit Surface (
                      RDGE::Int32  width,
                      RDGE::Int32  height,
                      RDGE::Int32  depth,
                      RDGE::UInt32 r_mask = DEFAULT_R_MASK,
                      RDGE::UInt32 g_mask = DEFAULT_G_MASK,
                      RDGE::UInt32 b_mask = DEFAULT_B_MASK,
                      RDGE::UInt32 a_mask = DEFAULT_A_MASK
                     );

    //! \brief Surface ctor
    //! \details Create an RGB surface from pixel data.  The pixel data is
    //!          provided through an r-value unique_ptr so the object can take
    //!          ownership as the SDL documentation states the pixel data
    //!          cannot be de-allocated until the surface has been freed
    //! \param [in] pixels R-value reference unique pointer to pixel data
    //! \param [in] width Width of the surface
    //! \param [in] height Height of the surface
    //! \param [in] depth Pixel depth (bits per pixel)
    //! \param [in] pitch The length of a row of pixels in bytes
    //! \param [in] r_mask Red mask for the pixels
    //! \param [in] g_mask Green mask for the pixels
    //! \param [in] b_mask Blue mask for the pixels
    //! \param [in] a_mask Alpha mask for the pixels
    //! \note If depth is 4 or 8 bits, and empty palette is allocated.  If
    //!       greater than 8 bits, the pixel format is set using the RGBA
    //!       mask parameters
    //! \see https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    //! \throws Image initialization failed
    explicit Surface (
                      std::unique_ptr<unsigned char[]>&& pixels,
                      RDGE::Int32                        width,
                      RDGE::Int32                        height,
                      RDGE::Int32                        depth,
                      RDGE::Int32                        pitch,
                      RDGE::UInt32                       r_mask,
                      RDGE::UInt32                       g_mask,
                      RDGE::UInt32                       b_mask,
                      RDGE::UInt32                       a_mask
                     );

    //! \brief Surface dtor
    ~Surface (void);

    //! \brief Surface Copy ctor
    //! \details Non-copyable
    Surface (const Surface&) = delete;

    //! \brief Surface Move ctor
    //! \details Transfers ownership of pointer
    Surface (Surface&& rhs) noexcept;

    //! \brief Surface Copy Assignment Operator
    //! \details Non-copyable
    Surface& operator= (const Surface&) = delete;

    //! \brief Surface Move Assignment Operator
    //! \details Transfers ownership of pointer
    Surface& operator= (Surface&& rhs) noexcept;

    //! \brief Return the raw SDL_Surface pointer
    //! \details Raw pointer is returned so caller must ensure
    //!          Surface object will not fall out of scope
    //! \return Raw pointer to an SDL_Surface
    SDL_Surface* RawPtr (void) const noexcept { return m_surface; }

    //! \brief Get the width of the surface
    //! \return Width as an unsigned int
    RDGE::UInt32 Width (void) const noexcept;

    //! \brief Get the height of the surface
    //! \return Height as an unsigned int
    RDGE::UInt32 Height (void) const noexcept;

    //! \brief Get the size (width and height) of the surface
    //! \return Size structure
    RDGE::Graphics::Size Size (void) const noexcept;

private:
    SDL_Surface*                     m_surface;
    std::unique_ptr<unsigned char[]> m_pixelData;
};

} // namespace RDGE
