#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/spritesheet_region.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/strings.hpp>

#include <SDL_assert.h>

#include <sstream>

// debug
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>

namespace rdge {

SpriteLayer::SpriteLayer (const tilemap::Layer& def,
                          const SpriteSheet& spritesheet,
                          float scale)
    : m_spriteCapacity(def.objects.size() + 100)
{
    // !! IMPORTANT !!
    //
    // 1) sprite count from the objects.size() is incorrect, as there
    //    could be objects that are not sprites.  Also, we may want to pad
    //    the amount to accommodate for adding sprites later.
    //    Perhaps another parameter.
    //
    // 2) Cannot reallocate b/c the list pointers will all be invalidated and
    //    pointers to the sprite_data will be stored by different objects.
    //    For now we'll enforce a strict limit, but it can be later extended to
    //    be more dynamic using a small block allocator.
    RDGE_CALLOC(m_sprites, m_spriteCapacity, nullptr);

    Texture t(*spritesheet.surface);
    if (std::find(textures.begin(), textures.end(), t) == textures.end())
    {
        t.unit_id = textures.size();
        textures.emplace_back(t);
    }

    for (const auto& obj : def.objects)
    {
        if (obj.type != tilemap::ObjectType::SPRITE)
        {
            continue;
        }

        const auto& region = spritesheet.regions[obj.m_gid - 1].value;
        auto& sprite = m_sprites[m_spriteCount];
        sprite.index = m_spriteCount++;
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

        // sorted insert according to render order
        auto it = m_list.begin();
        for (; it != m_list.end(); ++it)
        {
            if (it->pos.y > sprite.pos.y)
            {
                break;
            }
        }

        m_list.insert(it, sprite);
    }
}

SpriteLayer::~SpriteLayer (void) noexcept
{
    RDGE_FREE(m_sprites, nullptr);
}

SpriteLayer::SpriteLayer (SpriteLayer&& other) noexcept
    : m_list(std::move(other.m_list))
    , m_sprites(other.m_sprites)
    , m_spriteCount(other.m_spriteCount)
    , m_spriteCapacity(other.m_spriteCapacity)
    , m_color(other.m_color)
    , textures(std::move(other.textures))
{
    other.m_sprites = nullptr;
}

SpriteLayer&
SpriteLayer::operator= (SpriteLayer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_list = std::move(rhs.m_list);
        m_spriteCount = rhs.m_spriteCount;
        m_spriteCapacity = rhs.m_spriteCapacity;
        m_color = rhs.m_color;
        this->textures = std::move(rhs.textures);

        std::swap(m_sprites, rhs.m_sprites);
    }

    return *this;
}

sprite_data*
SpriteLayer::AddSprite (const math::vec2& pos,
                        uint32 id,
                        const SpriteSheet& spritesheet,
                        float scale)
{
    if (m_spriteCount == m_spriteCapacity)
    {
        RDGE_THROW("SpriteLayer count is at capacity");
    }

    Texture t(*spritesheet.surface);
    if (std::find(textures.begin(), textures.end(), t) == textures.end())
    {
        t.unit_id = textures.size();
        ILOG() << "found"
               << " t.unit_id=" << t.unit_id
               << " textures.size=" << textures.size();
        textures.emplace_back(t);
    }

    const auto& region = spritesheet.regions[id].value;
    auto& sprite = m_sprites[m_spriteCount];
    sprite.index = m_spriteCount++;
    sprite.pos = pos * scale;

    sprite.pos.x += region.sprite_offset.x * scale;
    sprite.pos.y += (region.size.h - region.sprite_size.h - region.sprite_offset.y) * scale;

    sprite.size = region.sprite_size * scale;
    sprite.depth = 1.f;
    sprite.color = color::WHITE;
    sprite.uvs = region.coords;
    sprite.tid = t.unit_id;

    ILOG() << "Add Sprite:"
           << " tid=" << sprite.tid
           << " pos=" << sprite.pos;

    // sorted insert according to render order
    auto it = m_list.begin();
    for (; it != m_list.end(); ++it)
    {
        if (it->pos.y > sprite.pos.y)
        {
            break;
        }
    }

    m_list.insert(it, sprite);

    return &sprite;
}

void
SpriteLayer::Draw (SpriteBatch& renderer, const OrthographicCamera& camera)
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

std::ostream&
operator<< (std::ostream& os, SpriteRenderOrder value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (SpriteRenderOrder value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(SpriteRenderOrder::INVALID)
        CASE(SpriteRenderOrder::TOPDOWN)
        CASE(SpriteRenderOrder::INDEX)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, SpriteRenderOrder& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "topdown") { out = SpriteRenderOrder::TOPDOWN; return true; }
    else if (s == "index")   { out = SpriteRenderOrder::INDEX;   return true; }

    return false;
}

} // namespace rdge
