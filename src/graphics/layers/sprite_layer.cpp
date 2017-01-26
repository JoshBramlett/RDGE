#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

namespace rdge {

SpriteLayer::SpriteLayer (uint16 num_sprites, std::shared_ptr<Shader> shader)
    : renderer(std::make_shared<SpriteBatch>(num_sprites, std::move(shader)))
{
    SDL_assert(this->renderer != nullptr);

    this->sprites.reserve(num_sprites);
}

SpriteLayer::SpriteLayer (std::shared_ptr<SpriteBatch> render_target)
    : renderer(std::move(render_target))
{
    SDL_assert(this->renderer != nullptr);

    this->sprites.reserve(this->renderer->Capacity());
}

void
SpriteLayer::AddSprite (std::shared_ptr<ISprite> sprite)
{
    SDL_assert(sprite != nullptr);

    sprite->SetRenderTarget(*this->renderer);
    this->sprites.emplace_back(std::move(sprite));
}

void
SpriteLayer::Draw (void)
{
    this->renderer->PrepSubmit();

    for (const auto& sprite : this->sprites)
    {
        sprite->Draw(*this->renderer);
    }

    this->renderer->Flush();
}

void
SpriteLayer::OverrideSpriteDepth (float depth)
{
    for (auto& sprite : this->sprites)
    {
        sprite->SetDepth(depth);
    }
}

} // namespace rdge
