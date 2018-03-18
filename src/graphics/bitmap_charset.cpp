#include <rdge/graphics/bitmap_charset.hpp>
#include <rdge/assets/bitmap_font.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <memory>
#include <cctype>

namespace rdge {

GlyphLayout::GlyphLayout (std::string text,
                          const math::vec2& pos,
                          const BitmapCharset& charset,
                          float scale,
                          const rdge::color& color,
                          float wrap_width,
                          float depth)
    : m_text(std::move(text))
    , m_xform(math::mat4::translation(pos))
    , m_color(color)
    , m_scale(scale)
    , m_wrap(wrap_width)
    , m_depth(depth)
{
    Rebuild(charset);
}

void
GlyphLayout::SetText (std::string text, const BitmapCharset& charset)
{
    m_text = std::move(text);
    Rebuild(charset);
}

void
GlyphLayout::SetPosition (const math::vec2& pos)
{
    m_xform = math::mat4::translation(pos);
}

void
GlyphLayout::SetDefaultColor (const rdge::color& color)
{
    m_color = color;
    for (auto& sprite : this->sprites)
    {
        // TODO This will need to be updated when markup is supported
        sprite.color = m_color;
    }
}

void
GlyphLayout::SetDepth (float depth)
{
    m_depth = depth;
    for (auto& sprite : this->sprites)
    {
        sprite.depth = m_depth;
    }
}

void
GlyphLayout::Rebuild (const BitmapCharset& charset)
{
    rdge::trim(m_text);
    SDL_assert(m_text.size() > 0);
    this->size = math::vec2(0.f, 0.f);
    this->line_count = 1;
    this->sprites.clear();
    this->sprites.reserve(m_text.size());

    math::vec2 cursor(0.f, 0.f);
    sprite_data sprite;
    sprite.depth = m_depth;
    sprite.color = m_color;

    size_t last_break_index = 0;
    size_t line_index = 0;
    for (size_t i = 0; i < m_text.size(); i++)
    {
        char c = m_text[i];
        SDL_assert(c >= 0);
        SDL_assert(c < static_cast<int32>(charset.glyphs.size()));

        const auto& g = charset.glyphs[c];
        SDL_assert(g.page >= 0);
        SDL_assert(g.page < static_cast<int32>(charset.textures.size()));

        if ((cursor.x + (g.x_advance * m_scale)) > m_wrap)
        {
            // if a single word is longer than the wrap width allow it to pass
            // through - unless it's a break char, and in that case process it.
            bool is_break = std::isspace(c);
            if (last_break_index > 0 || is_break)
            {
                if (is_break)
                {
                    // break falls on the end of a line
                    line_index = 0;
                }
                else
                {
                    // break falls prior to the end of the line - reset to the
                    // start of the current word.
                    //
                    // NOTE Not a fan of popping elements and resetting i, but
                    //      simply moving the positions to a new line doesn't work
                    //      because each glyph has an offset included in it's
                    //      position, so the text wouldn't align correctly.
                    SDL_assert(line_index > 0);
                    size_t num_traverse = line_index - last_break_index;
                    for (size_t j = 0; j < num_traverse; j++)
                    {
                        this->sprites.pop_back();
                    }

                    line_index = num_traverse + 1;
                    i -= num_traverse;
                }

                this->line_count++;
                this->size.w = std::max(this->size.w, cursor.x);
                this->size.y += (charset.line_height * m_scale);

                cursor.x = 0;
                cursor.y -= (charset.line_height * m_scale);
                last_break_index = 0;

                continue;
            }
        }

        if (std::isspace(c))
        {
            // ignore any preceeding whitespace on a line
            if (line_index == 0)
            {
                continue;
            }

            last_break_index = line_index;
            line_index++;
        }
        else
        {
            line_index++;
        }

        sprite.pos = cursor + (g.offset * m_scale);
        sprite.size = (g.size * m_scale);
        sprite.uvs = g.uvs;
        sprite.tid = charset.textures[g.page].unit_id;
        this->sprites.push_back(sprite);

        cursor.x += (g.x_advance * m_scale);
    }

    DLOG() << "GlyphLayout Built:"
           << " text=\"" << m_text << "\""
           << " lines=" << this->line_count
           << " sprites=" << this->sprites.size()
           << " render_size=" << this->size;
}

BitmapCharset::BitmapCharset (const BitmapFont& font, float scale)
    : line_height(font.line_height * scale)
    , baseline(font.baseline * scale)
    , shader(2.f, scale)
    //, shader(2.f, scale, color::BLACK, 0.15f)
    //, shader(2.f, scale, color::BLACK, 0.15f, math::vec2(1.f / -180.f, 1.f / 180.f))
{
    this->textures.reserve(font.surfaces.size());
    for (const auto& surface : font.surfaces)
    {
        // TODO Need a way to programmatically determine if this is a distance
        //      field font.  Could make it part of the asset processing.
        //      Also, libgdx says to use mipmaps if making the text smaller.
        Texture t(*surface, TextureFilter::LINEAR, TextureFilter::LINEAR);
        t.unit_id = this->textures.size();
        this->textures.emplace_back(std::move(t));
    }

    std::vector<glyph>(font.glyphs.size()).swap(this->glyphs);
    for (const auto& g_in : font.glyphs)
    {
        auto& g_out = this->glyphs.at(g_in.id);
        g_out.uvs = g_in.coords;
        g_out.size = g_in.size * scale;
        g_out.offset.x = g_in.offset.x * scale;
        g_out.offset.y = (font.line_height - (g_in.offset.y + g_in.size.h)) * scale;
        g_out.x_advance = g_in.x_advance * scale;
        g_out.page = static_cast<int8>(g_in.page);
    }

    this->shader.Build();
}

void
BitmapCharset::Draw (SpriteBatch& renderer, const GlyphLayout& layout)
{
    renderer.Prime(this->shader);
    renderer.PushTransformation(layout.m_xform);

    for (const auto& sprite : layout.sprites)
    {
        renderer.Draw(sprite);
    }

    renderer.PopTransformation();
    renderer.Flush(this->textures);
}

} // namespace rdge
