//! \headerfile <rdge/assets/surface.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/21/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <SDL_surface.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
struct screen_rect;
//!@}

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
    UNKNOWN = 0,
    BPP_24 = 24, //!< 24 bits per pixel (RGB)
    BPP_32 = 32  //!< 32 bits per pixel (RBGA)
};

//! \class Surface
//! \brief Wrapper for an SDL_Surface, which represents an image in memory
//! \details A surface provides a mechanism for loading images from disk.
//!          Surfaces are used for caching the image data, but cannot be used
//!          for rendering.  To render, Surfaces are used to build a
//!          \ref Texture object.  Maintains a non-atomic refcount making it
//!          able to be a shared resource.
class Surface
{
public:
    //! \brief Surface default ctor
    //! \details Take ownership of an existing SDL_Surface.
    //! \param [in] sdl_surface Native SDL_Surface
    explicit Surface (SDL_Surface* sdl_surface = nullptr);

    //! \brief Surface ctor
    //! \details Create a Surface from an image on disk.  If image depth is not
    //!          overridden the depth will be determined by the file.
    //! \param [in] filepath File path of the image to load
    //! \param [in] depth Optional requested depth
    //! \throws rdge::Exception Image initialization failed
    //! \throws rdge::SDLException SDL failed to create surface
    Surface (const std::string& filepath, PixelDepth depth = PixelDepth::UNKNOWN);

    //! \brief Surface ctor
    //! \details Create a Surface from preallocated pixel data (used with \ref PackFile).
    //! \param [in] pixel_data Pointer to raw pixel data
    //! \param [in] w Image width
    //! \param [in] h Image height
    //! \param [in] channels Number of color channels per pixel
    //! \throws rdge::Exception Image initialization failed
    //! \throws rdge::SDLException SDL failed to create surface
    Surface (void* pixel_data, int32 w, int32 h, int32 channels);

    //! \brief Surface dtor
    ~Surface (void) noexcept;

    //!@{
    //! \brief Copy and move enabled
    //! \details Copies are tracked by an internal refcount
    Surface (const Surface&);
    Surface& operator= (const Surface&);
    Surface (Surface&&) noexcept;
    Surface& operator= (Surface&&) noexcept;
    //!@}

    //!@{
    //! \brief User defined conversion to a raw SDL_Surface pointer
    //! \warning Pointer will be invalidated when parent object is destroyed
    explicit operator const SDL_Surface* (void) const noexcept { return m_surface; }
    explicit operator SDL_Surface* (void) const noexcept { return m_surface; }
    //!@}

    //!@{ Basic Surface properties
    bool IsEmpty (void) const noexcept;
    size_t Width (void) const noexcept;
    size_t Height (void) const noexcept;
    math::uivec2 Size (void) const noexcept;
    PixelDepth Depth (void) const noexcept;
    //!@}

    //! \brief Get the internal pixel format of the surface
    //! \details Value represents the SDL_PixelFormatEnum value
    //! \returns Pixel format enumeration value
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    uint32 PixelFormat (void) const noexcept;

    //! \brief Change the internal pixel format of the surface
    //! \param [in] pixel_format SDL_PixelFormatEnum value to change to
    //! \throws rdge::Exception If surface is invalid
    //! \throws rdge::SDLException If SDL failed the conversion
    //! \see https://wiki.libsdl.org/SDL_PixelFormatEnum
    void ChangePixelFormat (uint32 pixel_format);

    //! \brief Create a Surface from a sub-region in the current Surface
    //! \details Useful for pulling a single region from a \ref SpriteSheet.
    //!          Note The managed surface is unmodified.
    //! \param [in] clip Sub-surface coordinates
    //! \returns New Surface object
    Surface CreateSubSurface (const screen_rect& clip);

private:
    SDL_Surface* m_surface = nullptr;
};

//! \brief PixelDepth stream output operator
std::ostream& operator<< (std::ostream&, PixelDepth);

} // namespace rdge
