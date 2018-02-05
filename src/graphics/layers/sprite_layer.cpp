#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/util/strings.hpp>

#include <SDL_assert.h>

#include <sstream>

// NewSpriteLayer
#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/spritesheet_region.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/logger.hpp>
#include <algorithm>

// debug
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>

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

// NOTE: sprite count from the objects.size() is incorrect, as there
//       could be objects that are not sprites.  Also, we may want to pad
//       the amount to accommodate for adding sprites later.
//       Perhaps another parameter.
NewSpriteLayer::NewSpriteLayer (const tilemap::Layer& def,
                                const SpriteSheet& spritesheet,
                                float scale)
    : m_spriteCount(def.objects.size())
{
    RDGE_CALLOC(m_sprites, m_spriteCount, nullptr);

    Texture t(*spritesheet.surface);
    if (std::find(textures.begin(), textures.end(), t) == textures.end())
    {
        t.unit_id = textures.size();
        textures.emplace_back(std::move(t));
    }

    size_t sprite_index = 0;
    for (const auto& obj : def.objects)
    {
        if (obj.type != tilemap::ObjectType::SPRITE)
        {
            continue;
        }

        const auto& region = spritesheet.regions[obj.m_gid - 1].value;
        auto& sprite = m_sprites[sprite_index];
        sprite.index = sprite_index;
        sprite.pos = obj.position * scale;

        sprite.pos.x += region.sprite_offset.x * scale;
        sprite.pos.y -= (region.size.h - region.sprite_size.h - region.sprite_offset.y) * scale;
        sprite.pos.y *= -1.f;

        sprite.size = region.sprite_size * scale;
        sprite.depth = 1.f;
        sprite.color = color::WHITE;
        sprite.uvs = region.coords;
        sprite.tid = t.unit_id;
        //obj.m_rotation;
        //obj.visible;

        sprite_index++;

        m_list.push_back(sprite);
    }
}

NewSpriteLayer::~NewSpriteLayer (void) noexcept
{
    RDGE_FREE(m_sprites, nullptr);
}

NewSpriteLayer::NewSpriteLayer (NewSpriteLayer&& other) noexcept
    : m_list(std::move(other.m_list))
    , m_sprites(other.m_sprites)
    , m_spriteCount(other.m_spriteCount)
    , m_color(other.m_color)
    , textures(std::move(other.textures))
{
    other.m_sprites = nullptr;
}

NewSpriteLayer&
NewSpriteLayer::operator= (NewSpriteLayer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_list = std::move(rhs.m_list);
        m_spriteCount = rhs.m_spriteCount;
        m_color = rhs.m_color;
        this->textures = std::move(rhs.textures);

        std::swap(m_sprites, rhs.m_sprites);
    }

    return *this;
}

void
NewSpriteLayer::Draw (SpriteBatch& renderer, const OrthographicCamera& camera)
{
    rdge::Unused(camera);
    renderer.PrepSubmit();

    for (const auto& sprite : m_list)
    {
        math::vec2 hi (sprite.pos.x + sprite.size.w, sprite.pos.y + sprite.size.h);
        physics::aabb r(sprite.pos, hi);
        debug::DrawWireFrame(r, color::RED);

        renderer.Submit(sprite);
    }

    renderer.Flush(this->textures);
}

} // namespace rdge
