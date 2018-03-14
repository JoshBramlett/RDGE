#include <rdge/graphics/bitmap_charset.hpp>
#include <rdge/assets/bitmap_font.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <memory>

namespace rdge {

BitmapCharset::BitmapCharset (const BitmapFont& font, float scale)
    //: shader(2.f, 16.f)
    //: shader(2.f, 16.f, color::BLACK, 0.15f)
    : shader(2.f, 16.f, color::BLACK, 0.15f, math::vec2(1.f / -180.f, 1.f / 180.f))
{
    scale *= 4.f;
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

    // TODO Refactor
    //this->shader = new DistanceFieldOutlineShader(color::RED, 0.25f);
    this->shader.Build();
}

void
BitmapCharset::Draw (SpriteBatch& renderer, const std::string& text, const math::vec2& pos)
{
    renderer.Prime(this->shader);

    sprite_data sprite;
    sprite.depth = 0.f;
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

        renderer.Draw(sprite);
        cursor.x += g.x_advance;
    }

    renderer.Flush(m_textures);
}

} // namespace rdge
