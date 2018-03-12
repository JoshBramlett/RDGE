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

    static std::string f_src;
    if (f_src.empty())
    {
        std::ostringstream frag;
        frag << "#version 330 core\n"
             //
             << "layout (location = 0) out vec4 color;\n"
             //
             << "uniform sampler2D " << SpriteBatch::U_SAMPLER_ARRAY
                << "[" << Shader::MaxFragmentShaderUnits() << "];\n"
             //
             << "in vertex_attributes\n"
             << "{\n"
             << "  vec4 pos;\n"
             << "  vec2 uv;\n"
             << "  flat uint tid;\n"
             << "  vec4 color;\n"
             << "} vertex;\n"
             //
             << "const float smoothing = 1.0/16.0;\n"
             //
             << "void main()\n"
             << "{\n"
             << "  float distance = texture("
                << SpriteBatch::U_SAMPLER_ARRAY << "[vertex.tid], vertex.uv).a;\n"
             << "  float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);\n"
             << "  color = vec4(vertex.color.rgb, vertex.color.a * alpha);\n"
             << "}\n";

        f_src = frag.str();
    }

    this->shader = Shader(SpriteBatch::DefaultShader(ShaderType::VERTEX), f_src);
    std::vector<int32> texture_units(Shader::MaxFragmentShaderUnits());
    int32 n = 0;
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    this->shader.Enable();
    this->shader.SetUniformValue(SpriteBatch::U_SAMPLER_ARRAY, texture_units.size(), texture_units.data());
    this->shader.Disable();
}

void
BitmapCharset::Draw (const OrthographicCamera& camera, SpriteBatch& renderer, const std::string& text, const math::vec2& pos)
{
    renderer.Prime(camera, this->shader);

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

        renderer.Submit(sprite);
        cursor.x += g.x_advance;
    }

    renderer.Flush(m_textures);
}

} // namespace rdge
