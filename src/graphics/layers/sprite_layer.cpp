#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <sstream>
#include <memory>

// debug
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>

namespace rdge {

SpriteLayer::SpriteLayer (uint16 capacity)
    : m_spriteCapacity(capacity)
{
    if (RDGE_UNLIKELY(!RDGE_TCALLOC(m_sprites, m_spriteCapacity, memory_bucket_graphics)))
    {
        RDGE_THROW("Memory allocation failed");
    }
}

SpriteLayer::SpriteLayer (const tilemap::Layer& def, float scale)
    : m_spriteCapacity(def.objectgroup.objects.size())
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
    if (RDGE_UNLIKELY(!RDGE_TCALLOC(m_sprites, m_spriteCapacity, memory_bucket_graphics)))
    {
        RDGE_THROW("Memory allocation failed");
    }

    uint32 unit_id;
    {
        Texture t(*def.objectgroup.spritesheet->surface);
        const auto& it = std::find(textures.begin(), textures.end(), t);
        if (it == textures.end())
        {
            t.unit_id = textures.size();
            unit_id = t.unit_id;
            textures.emplace_back(std::move(t));
        }
        else
        {
            unit_id = it->unit_id;
        }
    }

    for (const auto& obj : def.objectgroup.objects)
    {
        if (obj.type != tilemap::ObjectType::SPRITE)
        {
            continue;
        }

        const auto& region = def.objectgroup.spritesheet->regions[obj.sprite.gid].value;
        auto& sprite = m_sprites[m_spriteCount];
        sprite.index = m_spriteCount++;

        // update position to accomodate for trimming
        sprite.pos = obj.pos * scale;
        sprite.pos.x += region.sprite_offset.x * scale;
        sprite.pos.y += region.sprite_offset.y * scale;

        sprite.size = region.sprite_size * scale;
        sprite.depth = 0.f;
        sprite.color = color::WHITE;
        sprite.uvs = region.coords;
        sprite.tid = unit_id;
        //obj.m_rotation;
        //obj.visible;

        if (sprite.size.w > m_padding.w)
        {
            m_padding.w = sprite.size.w;
        }

        if (sprite.size.h > m_padding.h)
        {
            m_padding.h = sprite.size.h;
        }

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
    RDGE_FREE(m_sprites, memory_bucket_graphics);
}

SpriteLayer::SpriteLayer (SpriteLayer&& other) noexcept
    : m_list(std::move(other.m_list))
    , m_sprites(other.m_sprites)
    , m_spriteCount(other.m_spriteCount)
    , m_spriteCapacity(other.m_spriteCapacity)
    , m_color(other.m_color)
    , m_padding(other.m_padding)
    , name(std::move(other.name))
    , textures(std::move(other.textures))
#ifdef RDGE_DEBUG
    , debug_overlay(other.debug_overlay)
#endif
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
        m_padding = rhs.m_padding;
        this->name = std::move(rhs.name);
        this->textures = std::move(rhs.textures);
#ifdef RDGE_DEBUG
        this->debug_overlay = rhs.debug_overlay;
#endif

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

    uint32 unit_id;
    {
        Texture t(*spritesheet.surface);
        const auto& it = std::find(textures.begin(), textures.end(), t);
        if (it == textures.end())
        {
            t.unit_id = textures.size();
            unit_id = t.unit_id;
            textures.emplace_back(std::move(t));
        }
        else
        {
            unit_id = it->unit_id;
        }
    }

    const auto& region = spritesheet.regions[id].value;
    auto& sprite = m_sprites[m_spriteCount];
    sprite.index = m_spriteCount++;

    // update position to accomodate for trimming
    sprite.pos = pos * scale;
    sprite.pos.x += region.sprite_offset.x * scale;
    sprite.pos.y += region.sprite_offset.y * scale;

    sprite.size = region.sprite_size * scale;
    sprite.depth = 0.f;
    sprite.color = color::WHITE;
    sprite.uvs = region.coords;
    sprite.tid = unit_id;

    if (sprite.size.w > m_padding.w)
    {
        m_padding.w = sprite.size.w;
    }

    if (sprite.size.h > m_padding.h)
    {
        m_padding.h = sprite.size.h;
    }

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
#ifdef RDGE_DEBUG
    this->debug_overlay.sprites_drawn = 0;
    if (this->debug_overlay.hide_layer)
    {
        return;
    }
#endif

    // buffer the camera bounds by max padding
    auto frame_bounds = camera.bounds;
    frame_bounds.fatten(m_padding.w, m_padding.h);

    renderer.SetView(camera);
    renderer.PrepSubmit();

    m_list.sort([](const auto& a, const auto& b) { return a.pos.y > b.pos.y; });
    for (const auto& sprite : m_list)
    {
        // NOTE Culling by an AABB intersection test may be sub-optimal
        //      when there are a lot of sprites outside the camera bounds.
        //      Since the list is sorted by the y-coordinate, it might be
        //      beneficial to cache the first sprite rendered on the previous
        //      frame.  Then do a quick backwards traversal to see if there
        //      are more that must be rendered versus the prior frame.
        physics::aabb box(sprite.pos, sprite.size.w, sprite.size.h);
        if (frame_bounds.intersects_with(box))
        {
#ifdef RDGE_DEBUG
            if (this->debug_overlay.draw_sprite_frames)
            {
                debug::DrawWireFrame(box, debug::settings::graphics::colors::sprites);
            }

            this->debug_overlay.sprites_drawn++;
#endif
            renderer.Submit(sprite);
        }
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
