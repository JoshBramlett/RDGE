#include <rdge/graphics/text.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <memory>

namespace rdge {

Text::Text (shared_asset<BitmapFont> font)
    : m_font(std::move(font))
{
    m_textures.reserve(m_font->surfaces.size());
    for (const auto& surface : m_font->surfaces)
    {
        Texture t(*surface);
        t.unit_id = m_textures.size();
        m_textures.emplace_back(std::move(t));
    }
}

void
Text::Draw (SpriteBatch& renderer, const std::string& text, const math::vec2& pos)
{
    renderer.PrepSubmit();
    // TODO pass scale in ctor, and cache glyphs with scaled values
    float scale = 4.f;

    sprite_data sprite;
    sprite.pos = pos * scale;
    sprite.depth = 1.f;
    sprite.color = this->color;

    for (char c : text)
    {
        const auto& glyph = (*m_font)[c];
        sprite.size = glyph.size * scale;
        sprite.uvs = glyph.coords;
        sprite.tid = m_textures[glyph.page].unit_id;

        renderer.Submit(sprite);

        sprite.pos.x += glyph.size.w;
        sprite.pos.x += (m_font->pad_right * scale) + (m_font->pad_left * scale);
        sprite.pos.x += glyph.x_advance * scale;
    }

    renderer.Flush(m_textures);
}

} // namespace rdge
