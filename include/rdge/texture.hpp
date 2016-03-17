//! \headerfile <rdge/texture.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/28/2015
//! \bug

/* TODO - Methods/functionality missing from libSDL2pp
 *
 *  1)  Reading texture from stream using IMG_LoadTexture_RW
 *  2)  Locking the texture and overwriting it's contents using SDL_LockTexture.
 *      This is something I'll have to do more research on.  Lazyfoo has a
 *      tutorial on it.
 *  3)  Get/SetBlendMode, Get/SetAlphaMod, Get/SetColorMod, GetFormat, GetAccess
 *          - all things I'll need to figure out their purpose
 */

#pragma once

#include <string>

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/graphics/size.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \typedef SDLTextureUniquePtr
//! \details Proper unique_ptr type implementing SDL custom deleter
//! \note This wraps the native SDL_Texture, not the RDGE::Texture.  It's
//!       available as an alternative to the RAII object
using SDLTextureUniquePtr = std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>;

//! \brief Create unique_ptr wrapper for native SDL_Texture
//! \details Helper function to wrap an SDL_Texture in a unique_ptr
//!          with the proper custom deleter
//! \param [in] texture Pre-allocated SDL_Texture
//! \returns unique_ptr of provided SDL_Texture
inline auto CreateSDLTextureUniquePtr (SDL_Texture* texture) -> SDLTextureUniquePtr
{
    return SDLTextureUniquePtr(texture, SDL_DestroyTexture);
}

//! \class Texture
//! \details RAII complient wrapper for SDL_Texture
//! \brief Textures are stored in the GPU and should be used in leiu
//!        of \ref Surface whenever possible
class Texture
{
public:
    //! \brief Texture ctor
    //! \param [in] texture Pointer to SDL_Texture
    explicit Texture (SDL_Texture* texture);

    //! \brief Texture ctor
    //! \details Uses the SDL_image library to load from file
    //! \param [in] renderer Renderer used by the \ref RDGE::Window
    //! \param [in] file File path of the image to load
    //! \throws Image initialization failed
    explicit Texture (
                      SDL_Renderer*      renderer,
                      const std::string& file
                     );

    //! \brief Texture ctor
    //! \details Creates GPU texture from a memory stored surface
    //! \param [in] renderer Renderer used by the \ref RDGE::Window
    //! \param [in] surface SDL_Surface
    //! \throws Image initialization failed
    explicit Texture (
                      SDL_Renderer* renderer,
                      SDL_Surface*  surface
                     );

    //! \brief Texture dtor
    ~Texture (void);

    //! \brief Texture Copy ctor
    //! \details Non-copyable
    Texture (const Texture&) = delete;

    //! \brief Texture Move ctor
    //! \details Transfers ownership of pointer
    Texture (Texture&& rhs) noexcept;

    //! \brief Texture Copy Assignment Operator
    //! \details Non-copyable
    Texture& operator= (const Texture&) = delete;

    //! \brief Texture Move Assignment Operator
    //! \details Transfers ownership of pointer
    Texture& operator= (Texture&& rhs) noexcept;

    //! \brief Return the SDL_Texture pointer
    //! \details Raw pointer is returned so caller must ensure
    //!          Texture object will not fall out of scope
    //! \return Raw pointer to an SDL_Texture
    SDL_Texture* RawPtr (void) const noexcept { return m_texture; }

    //! \brief Get the width of the texture
    //! \return Width as an unsigned int
    //! \throws Failed to query width
    RDGE::UInt32 Width (void) const;

    //! \brief Get the height of the texture
    //! \return Height as an unsigned int
    //! \throws Failed to query height
    RDGE::UInt32 Height (void) const;

    //! \brief Get the size (width and height) of the texture
    //! \return Size structure
    //! \throws Failed to query size
    RDGE::Graphics::Size Size (void) const;

private:
    SDL_Texture*  m_texture;
};

} // namespace RDGE
