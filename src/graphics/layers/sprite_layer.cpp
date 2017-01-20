#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

namespace rdge {

SpriteLayer::SpriteLayer (uint16 num_sprites, std::shared_ptr<Shader> shader)
    : renderer(num_sprites, std::move(shader))
{
    this->sprites.reserve(num_sprites);
}

void
SpriteLayer::AddSprite (std::shared_ptr<ISprite> sprite)
{
    SDL_assert(sprite != nullptr);

    // TODO I think push_back and emplace_back should be equivalent here and there's
    //      no extra copy with push_back.  Unfortunately there's nothing on the web
    //      for inserting smart pointers, so I'll have to measure and test myself.
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

void
SpriteLayer::OverrideSpriteDepth (float depth)
{
    for (auto& sprite : this->sprites)
    {
        sprite->SetDepth(depth);
    }
}

} // namespace rdge
