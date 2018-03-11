#include <rdge/graphics/bitmap_charset.hpp>
#include <rdge/assets/bitmap_font.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <memory>

namespace rdge {

BitmapCharset::BitmapCharset (const BitmapFont& font, float scale)
{
    m_textures.reserve(font.surfaces.size());
    for (const auto& surface : font.surfaces)
    {
        // TODO Need a way to programmatically determine if this is a distance
        //      field font.  Could make it part of the asset processing.
        //      Also, libgdx says to use mipmaps if making the text smaller.
        Texture t(*surface, TextureFilter::LINEAR, TextureFilter::LINEAR);
        t.unit_id = m_textures.size();
        m_textures.emplace_back(std::move(t));
    }

    std::vector<glyph>(font.glyphs.size()).swap(m_glyphs);
    for (const auto& g_in : font.glyphs)
    {
        auto& g_out = m_glyphs.at(g_in.id);
        g_out.uvs = g_in.coords;
        g_out.size = g_in.size * scale;
        g_out.offset.x = g_in.offset.x * scale;
        g_out.offset.y = (font.line_height - (g_in.offset.y + g_in.size.h)) * scale;
        g_out.x_advance = g_in.x_advance * scale;
        g_out.page = static_cast<int8>(g_in.page);
    }
}

void
BitmapCharset::Draw (SpriteBatch& renderer, const std::string& text, const math::vec2& pos)
{
    renderer.PrepSubmit();

    sprite_data sprite;
    sprite.depth = 1.f;
    sprite.color = this->color;

    math::vec2 cursor = pos;
    for (char c : text)
    {
        // TODO bounds checking and make sure glyph is valid
        const auto& g = m_glyphs[c];
        sprite.pos = cursor + g.offset;
        sprite.size = g.size;
        sprite.uvs = g.uvs;
        sprite.tid = m_textures[g.page].unit_id;

        renderer.Submit(sprite);
        cursor.x += g.x_advance;
    }

    renderer.Flush(m_textures);
}

} // namespace rdge
