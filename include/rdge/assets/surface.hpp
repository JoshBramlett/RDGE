//! \headerfile <rdge/assets/surface.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/21/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/size.hpp>

#include <SDL.h>

#include <string>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \typedef SDLSurfaceUniquePtr
//! \details Encosing unique_ptr type that includes the SDL custom deleter
using SDLSurfaceUniquePtr = std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>;

//! \brief Create SDL_Surface unique_ptr with a proper deleter
//! \param [in] surface Pre-allocated SDL_Surface
//! \returns unique_ptr of provided SDL_Surface
inline auto CreateManagedSDLSurface (SDL_Surface* surface) -> SDLSurfaceUniquePtr
{
    return SDLSurfaceUniquePtr(surface, SDL_FreeSurface);
}

//! \enum PixelDepth
//! \brief Supported pixel depth values
enum class PixelDepth : int32
{
    BPP_24 = 24, //!< 24 bytes per pixel (RGB)
    BPP_32 = 32  //!< 32 bytes per pixel (RBGA)
};

//! \class Surface
//! \brief Wrapper for an SDL_Surface, which represents an image in memory
//! \details A surface provides a mechanism for loading images from disk and
//!          creating an image from pixel data.  Surfaces can be cached to
//!          provide faster lookup, but should not be used for rendering.
//!          Surfaces should be converted to textures for rendering.
//! \see rdge::Texture
class Surface
{
public:
    //! \brief Surface ctor
    //! \details Creates a surface from a native SDL_Surface.  The object will take
    //!          ownership of the SDL_Surface.
    //! \param [in] surface Pointer to SDL_Surface
    explicit Surface (SDL_Surface* surface);

    //! \brief Surface ctor
    //! \details Uses the SDL_image library to load from file
    //! \param [in] file File path of the image to load
    //! \throws rdge::SDLException Image initialization failed
    explicit Surface (const std::string& path);

    //! \brief Surface ctor
    //! \details Create a blank surface.
    //! \param [in] size Size of the surface
    //! \param [in] depth Pixel depth (bits per pixel)
    //! \throws rdge::SDLException Image initialization failed
    //! \note A depth of 24bpp creates an RGB surface, 32bpp creates an RGBA surface
    explicit Surface (const rdge::size& size, PixelDepth depth = PixelDepth::BPP_32);

    //! \brief Surface ctor
    //! \details Create a surface from pixel data.  The object will take ownership
    //!          of the pixel data.
    //! \param [in] pixels Unique pointer to pixel data
    //! \param [in] size Size of the surface
    //! \param [in] depth Pixel depth (bits per pixel)
    //! \throws rdge::Exception Pixel data is invalid
    //! \throws rdge::SDLException Image initialization failed
    //! \note A depth of 24bpp creates an RGB surface, 32bpp creates an RGBA surface
    explicit Surface (std::unique_ptr<uint8[]> pixels,
                      const rdge::size&        size,
                      PixelDepth               depth = PixelDepth::BPP_32);

    //! \brief Surface dtor
    ~Surface (void) noexcept;

    //!@{
    //! \brief Non-copyable and move enabled
    Surface (const Surface&) = delete;
    Surface& operator= (const Surface&) = delete;
    Surface (Surface&&) noexcept;
    Surface& operator= (Surface&&) noexcept;
    //!@}

    //! \brief User defined conversion to a raw SDL_Surface pointer
    //! \warning Be careful not to dereference the pointer after the
    //!          parent Surface object falls out of scope
    //! \returns const pointer to a native SDL_Surface
    explicit operator const SDL_Surface* (void) const noexcept
    {
        return m_surface;
    }

    //! \brief Get the width of the surface
    //! \return Surface width
    uint32 Width (void) const noexcept;

    //! \brief Get the height of the surface
    //! \return Surface height
    uint32 Height (void) const noexcept;

    //! \brief Get the size (width and height) of the surface
    //! \return Size structure
    rdge::size Size (void) const noexcept;

    //! \brief Get the internal pixel format of the surface
    //! \details Value represents the SDL_PixelFormatEnum value
    //! \return Pixel format enumeration value
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    uint32 PixelFormat (void) const noexcept;

    //! \brief Change the internal pixel format of the surface
    //! \param [in] pixel_format SDL_PixelFormatEnum value to change to
    //! \throws rdge::Exception If surface is invalid
    //! \throws rdge::SDLException If SDL failed the conversion
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    void ChangePixelFormat (uint32 pixel_format);

private:
    SDL_Surface*             m_surface = nullptr;
    std::unique_ptr<uint8[]> m_pixelData;
};

} // namespace rdge
