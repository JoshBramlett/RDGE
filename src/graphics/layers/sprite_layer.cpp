#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/util/strings.hpp>

#include <SDL_assert.h>

#include <sstream>

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

std::ostream&
operator<< (std::ostream& os, DrawOrder value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (DrawOrder value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(DrawOrder::INVALID)
        CASE(DrawOrder::TOPDOWN)
        CASE(DrawOrder::INDEX)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, DrawOrder& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "topdown") { out = DrawOrder::TOPDOWN; return true; }
    else if (s == "index")   { out = DrawOrder::INDEX;   return true; }

    return false;
}

} // namespace rdge
