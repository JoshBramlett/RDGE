//! \headerfile <rdge/surface.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/07/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/point.hpp>

#include <SDL.h>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Assets {

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
//! \brief Can be used as an RAII complient wrapper for SDL_Surface, or
//!        has functionality to be used as a shared_ptr.
//! \details Surface data is stored in ram so the pixel data within should
//!          not be used for rendering.  Rather, it should simply be used to
//!          cache an image asset and the pixel data should be transferred to
//!          the texture for rendering.
class Surface : public std::enable_shared_from_this<Surface>
{
public:
    //! \brief Surface ctor
    //! \param [in] surface Pointer to SDL_Surface
    explicit Surface (SDL_Surface* surface);

    //! \brief Surface ctor
    //! \details Uses the SDL_image library to load from file
    //! \param [in] file File path of the image to load
    //! \throws RDGE::SDLException Image initialization failed
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
    //! \throws RDGE::SDLException Image initialization failed
    explicit Surface (
                      RDGE::Int32  width,
                      RDGE::Int32  height,
                      RDGE::Int32  depth,
                      RDGE::UInt32 r_mask = BYTEORDER_R_MASK,
                      RDGE::UInt32 g_mask = BYTEORDER_G_MASK,
                      RDGE::UInt32 b_mask = BYTEORDER_B_MASK,
                      RDGE::UInt32 a_mask = BYTEORDER_A_MASK
                     );

    //! \brief Surface ctor
    //! \details Create an RGB surface from pixel data.  The pixel data is
    //!          provided through a unique_ptr so the object can take
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
    //! \note If depth is 4 or 8 bits, an empty palette is allocated.  If
    //!       greater than 8 bits, the pixel format is set using the RGBA
    //!       mask parameters
    //! \see https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    //! \throws RDGE::SDLException Image initialization failed
    explicit Surface (
                      std::unique_ptr<unsigned char[]> pixels,
                      RDGE::Int32                      width,
                      RDGE::Int32                      height,
                      RDGE::Int32                      depth,
                      RDGE::Int32                      pitch,
                      RDGE::UInt32                     r_mask,
                      RDGE::UInt32                     g_mask,
                      RDGE::UInt32                     b_mask,
                      RDGE::UInt32                     a_mask
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

    //! \brief Safely retrieve additional shared_ptr instance
    //! \returns Shared pointer of the current object
    //! \throws std::bad_weak_ptr If called when object is not managed
    //!         by a shared_ptr
    std::shared_ptr<Surface> GetSharedPtr (void) noexcept
    {
        return shared_from_this();
    }

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

    //! \brief Get the internal pixel format of the surface
    //! \details Will return the SDL_PixelFormatEnum value
    //! \return Pixel format enumeration value
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    RDGE::UInt32 PixelFormat (void) const noexcept;

    //! \brief Change the internal pixel format of the surface
    //! \details If there is no change from the current format, the call is a no-op.
    //! \param [in] pixel_format SDL_PixelFormatEnum value to change to
    //! \throws RDGE::Exception If surface is null
    //! \throws RDGE::SDLException If SDL failed the conversion
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    void ChangePixelFormat (RDGE::UInt32 pixel_format);

private:
    SDL_Surface*                     m_surface;
    std::unique_ptr<unsigned char[]> m_pixelData;
};

} // namespace Assets
} // namespace RDGE
