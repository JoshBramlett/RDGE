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
             //<< "const float smoothing = 1.0/16.0;\n"
             ////
             //<< "void main()\n"
             //<< "{\n"
             //<< "  float distance = texture("
                //<< SpriteBatch::U_SAMPLER_ARRAY << "[vertex.tid], vertex.uv).a;\n"
             //<< "  float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);\n"
             //<< "  color = vec4(vertex.color.rgb, vertex.color.a * alpha);\n"
             //<< "}\n";

//...
//const float outlineDistance; // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
//const vec4 outlineColor;
//...
//void main() {
    //float distance = texture2D(u_texture, v_texCoord).a;
    //float outlineFactor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
    //vec4 color = mix(outlineColor, v_color, outlineFactor);
    //float alpha = smoothstep(outlineDistance - smoothing, outlineDistance + smoothing, distance);
    //gl_FragColor = vec4(color.rgb, color.a * alpha);
//}

             // Outline distance - between 0.0 (thick) and 0.5 (no outline)
             << "const float smoothing = 1.0/16.0;\n"
             << "const float ol_dis = 0.25;\n"
             << "const vec4 ol_color = vec4(1.0, 0.0, 0.0, 1.0);\n"
             //
             << "void main()\n"
             << "{\n"
             << "  float distance = texture("
                << SpriteBatch::U_SAMPLER_ARRAY << "[vertex.tid], vertex.uv).a;\n"
             << "  float ol_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);\n"
             << "  vec4 mix_color = mix(ol_color, vertex.color, ol_factor);\n"
             << "  float alpha = smoothstep(ol_dis - smoothing, ol_dis + smoothing, distance);\n"
             << "  color = vec4(mix_color.rgb, mix_color.a * alpha);\n"
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
