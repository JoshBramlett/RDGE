#include <rdge/texture.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_image.h>

namespace RDGE {

Texture::Texture (SDL_Texture* texture)
    : m_texture(texture)
{ }

Texture::Texture (
                  SDL_Renderer*      renderer,
                  const std::string& file
                 )
{
    m_texture = IMG_LoadTexture(renderer, file.c_str());
    if (UNLIKELY(!m_texture))
    {
        SDL_THROW("Failed to load texture. file=" + file, "IMG_LoadTexture");
    }
}

Texture::Texture (
                  SDL_Renderer* renderer,
                  SDL_Surface*  surface
                 )
{
    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (UNLIKELY(!m_texture))
    {
        SDL_THROW("Failed to create texture from surface", "SDL_CreateTextureFromSurface");
    }
}

Texture::Texture (Texture&& rhs) noexcept
    : m_texture(rhs.m_texture)
{
    rhs.m_texture = nullptr;
}

Texture&
Texture::operator= (Texture&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (m_texture != nullptr)
        {
            SDL_DestroyTexture(m_texture);
        }

        m_texture = rhs.m_texture;
        rhs.m_texture = nullptr;
    }

    return *this;
}

Texture::~Texture (void)
{
    if (m_texture != nullptr)
    {
        SDL_DestroyTexture(m_texture);
    }
}

RDGE::UInt32
Texture::Width (void) const
{
    auto size = this->Size();
    return static_cast<RDGE::UInt32>(size.w);
}

RDGE::UInt32
Texture::Height (void) const
{
    auto size = this->Size();
    return static_cast<RDGE::UInt32>(size.h);
}

RDGE::Graphics::Size
Texture::Size (void) const
{
    RDGE::Int32 w = 0, h = 0;
    if (UNLIKELY(SDL_QueryTexture(m_texture, NULL, NULL, &w, &h) != 0))
    {
        SDL_THROW("Unable to get image size", "SDL_QueryTexture");
    }

    return RDGE::Graphics::Size(w, h);
}

} // namespace RDGE
