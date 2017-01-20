#include <rdge/graphics/sprite_group.hpp>
#include <rdge/math/functions.hpp>

#include <SDL_assert.h>

namespace rdge {

SpriteGroup::SpriteGroup (const math::mat4& transform)
    : transformation(transform)
{ }

void
SpriteGroup::SetRenderTarget (SpriteBatch& renderer)
{
    for (auto& sprite : this->sprites)
    {
        sprite->SetRenderTarget(renderer);
    }
}

void
SpriteGroup::Draw (SpriteBatch& renderer)
{
    renderer.PushTransformation(this->transformation);

    for (const auto& sprite : this->sprites)
    {
        sprite->Draw(renderer);
    }

    renderer.PopTransformation();
}

void
SpriteGroup::SetDepth (float depth)
{
    for (auto& sprite : this->sprites)
    {
        sprite->SetDepth(depth);
    }
}

void
SpriteGroup::AddSprite (std::shared_ptr<ISprite> sprite)
{
    // TODO The old version of group calculated the total renderable area in
    //      order to process the ray casting.  There has to be a way to work
    //      around that instead of seeing if the cursor position was inside the
    //      render box.  Also I'm not exposing a forced getter for position and
    //      size, so currently not an option.  The previous method also fails
    //      when transformations are performed on the group.
    //      Note: I may still need to calculate the total dimensions for
    //      collision detection.

    SDL_assert(sprite != nullptr);
    this->sprites.push_back(sprite);
}

// TODO Leaving this here because I don't want to forget about it.  When I find
//      an appropriate place it'll get moved
//void
//SpriteGroup::RotateOnCenter (float angle)
//{
    //m_transformation *= mat4::translation(vec3(m_size.x / 2.0f, m_size.y / 2.0f, 0.0f));
    //m_transformation *= mat4::rotation(angle, vec3(0.0f, 0.0f, 1.0f));
    //m_transformation *= mat4::translation(vec3(-m_size.x / 2.0f, -m_size.y / 2.0f, 0.0f));
//}

} // namespace rdge
