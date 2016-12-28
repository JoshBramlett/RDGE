#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

namespace {

using namespace rdge;

uint32
GetUniqueZIndex (void)
{
    static uint32 z_index = 0;
    return z_index++;
}

} // anonymous namespace

namespace rdge {

SpriteLayer::SpriteLayer (uint16 num_sprites, std::shared_ptr<Shader> shader)
    : renderer(num_sprites, std::move(shader))
    , z_index(GetUniqueZIndex())
{
    this->sprites.reserve(num_sprites);
}

SpriteLayer::SpriteLayer (SpriteLayer&& rhs) noexcept
    : renderer(std::move(rhs.renderer))
    , sprites(std::move(rhs.sprites))
{
    std::swap(this->z_index, rhs.z_index);
}

SpriteLayer&
SpriteLayer::operator= (SpriteLayer&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->renderer = std::move(rhs.renderer);
        this->sprites = std::move(rhs.sprites);
        std::swap(this->z_index, rhs.z_index);
    }

    return *this;
}

void
SpriteLayer::AddSprite (std::shared_ptr<ISprite> sprite)
{
    SDL_assert(sprite != nullptr);

    sprite->AmendDepthMask(DepthMask::convert(this->z_index));
    sprite->SetRenderTarget(this->renderer);
    this->sprites.push_back(sprite);
}

void
SpriteLayer::Draw (void)
{
    this->renderer.PrepSubmit();

    for (const auto& sprite : this->sprites)
    {
        sprite->Draw(this->renderer);
    }

    this->renderer.Flush();
}

} // namespace rdge
