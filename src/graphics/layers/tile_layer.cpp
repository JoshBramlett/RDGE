#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

namespace rdge {

        {
         "data":[],
         "height":60,
         "name":"background",
         "offsetx":560.790273556231,
         "offsety":332.826747720365,
         "opacity":1,
         "properties":
            {
             "cust_prop_bool":true,
             "cust_prop_color":"#ffec9cc6",
             "cust_prop_file":"overworld_obj.json",
             "cust_prop_float":3.14,
             "cust_prop_int":5,
             "cust_prop_string":"asdf"
            },
         "propertytypes":
            {
             "cust_prop_bool":"bool",
             "cust_prop_color":"color",
             "cust_prop_file":"file",
             "cust_prop_float":"float",
             "cust_prop_int":"int",
             "cust_prop_string":"string"
            },
         "type":"tilelayer",
         "visible":true,
         "width":60,
         "x":0,
         "y":0
        },

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

} // namespace rdge
