#include <rdge/graphics/shaders/sprite_batch_shader.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>

#include <sstream>
#include <vector>
#include <numeric> // std::iota

namespace rdge {

using namespace rdge::math;

void
SpriteBatchShader::Build (void)
{
    this->shader = ShaderProgram(GetVertexSource(), GetFragmentSource());

    // A requirement we impose on the fragment shader is to define a sampler2D array
    // with the element count equal to the maximum texture units available.  A vector
    // is filled with values { 0 ... MAX-1 } and set to the uniform.
    // TODO make this static
    std::vector<int32> slots(ShaderProgram::MaxTextureSlots());
    std::iota(slots.begin(), slots.end(), 0);

    this->shader.Enable();
    this->shader.SetUniformValue(SpriteBatch::U_SAMPLER_ARRAY, slots.size(), slots.data());
    this->shader.Disable();
}

const std::string&
SpriteBatchShader::GetVertexSource (void)
{
    static std::string v_src;
    if (v_src.empty())
    {
        using SB = SpriteBatch;
        std::ostringstream ss;
        ss << "#version 330 core\n"
           //
           << "layout (location = " << SB::VA_POS_INDEX   << ") in vec4 position;\n"
           << "layout (location = " << SB::VA_UV_INDEX    << ") in vec2 uv;\n"
           << "layout (location = " << SB::VA_TID_INDEX   << ") in uint tid;\n"
           << "layout (location = " << SB::VA_COLOR_INDEX << ") in vec4 color;\n"
           //
           << "uniform mat4 " << SB::U_PROJ_XF << ";\n"
           //
           << "out vertex_attributes\n"
           << "{\n"
           << "  vec4 pos;\n"
           << "  vec2 uv;\n"
           << "  flat uint tid;\n"
           << "  vec4 color;\n"
           << "} v_va;\n"
           //
           << "void main()\n"
           << "{\n"
           << "  v_va.pos = position;\n"
           << "  v_va.uv = uv;\n"
           << "  v_va.tid = tid;\n"
           << "  v_va.color = color;\n"
           << "  gl_Position = " << SB::U_PROJ_XF << " * position;\n"
           << "}\n";

        v_src = ss.str();
    }

    return v_src;
}

const std::string&
SpriteBatchShader::GetFragmentSource (void)
{
    static std::string f_src;
    if (f_src.empty())
    {
        using SB = SpriteBatch;
        auto max_units = ShaderProgram::MaxTextureSlots();
        std::ostringstream ss;
        ss << "#version 330 core\n"
           //
           << "layout (location = 0) out vec4 color;\n"
           //
           << "uniform sampler2D " << SB::U_SAMPLER_ARRAY << "[" << max_units << "];\n"
           //
           << "in vertex_attributes\n"
           << "{\n"
           << "  vec4 pos;\n"
           << "  vec2 uv;\n"
           << "  flat uint tid;\n"
           << "  vec4 color;\n"
           << "} v_va;\n"
           //
           << "void main()\n"
           << "{\n"
           << "  vec4 sample = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv);\n"
           << "  color = v_va.color * sample;\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

const std::string&
DistanceFieldShader::GetFragmentSource (void)
{
    static std::string f_src;
    if (f_src.empty())
    {
        using SB = SpriteBatch;
        auto max_units = ShaderProgram::MaxTextureSlots();
        std::ostringstream ss;
        ss << "#version 330 core\n"
           //
           << "layout (location = 0) out vec4 color;\n"
           //
           << "uniform sampler2D " << SB::U_SAMPLER_ARRAY << "[" << max_units << "];\n"
           //
           << "in vertex_attributes\n"
           << "{\n"
           << "  vec4 pos;\n"
           << "  vec2 uv;\n"
           << "  flat uint tid;\n"
           << "  vec4 color;\n"
           << "} v_va;\n"
           //
           << "const float s = 1.0 / 16.0;\n"  // smoothing ratio
           << "const float se_0 = 0.5 - s;\n"  // smoothing edge_0
           << "const float se_1 = 0.5 + s;\n"  // smoothing edge_1
           //
           << "void main()\n"
           << "{\n"
           << "  float d = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv).a;\n"
           << "  float alpha = smoothstep(se_0, se_1, d);\n"
           << "  color = vec4(v_va.color.rgb, v_va.color.a * alpha);\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

DistanceFieldOutlineShader::DistanceFieldOutlineShader (const color& c, float d)
    : m_color(static_cast<math::vec4>(c))
    , m_distance(math::clamp(d, MIN_DISTANCE, MAX_DISTANCE))
{ }

void
DistanceFieldOutlineShader::Build (void)
{
    SpriteBatchShader::Build();

    this->shader.Enable();
    this->shader.SetUniformValue(U_OUTLINE_COLOR, m_color);
    this->shader.SetUniformValue(U_OUTLINE_DISTANCE, m_distance);
    this->shader.Disable();
}

void
DistanceFieldOutlineShader::SetOutlineColor (const color& c)
{
    m_color = static_cast<math::vec4>(c);
    if (!this->shader.IsEmpty())
    {
        this->shader.Enable();
        this->shader.SetUniformValue(U_OUTLINE_COLOR, m_color);
        this->shader.Disable();
    }
}

void
DistanceFieldOutlineShader::SetOutlineDistance (float d)
{
    m_distance = math::clamp(d, MIN_DISTANCE, MAX_DISTANCE);
    if (!this->shader.IsEmpty())
    {
        this->shader.Enable();
        this->shader.SetUniformValue(U_OUTLINE_DISTANCE, m_distance);
        this->shader.Disable();
    }
}

const std::string&
DistanceFieldOutlineShader::GetFragmentSource (void)
{
    static std::string f_src;
    if (f_src.empty())
    {
        using SB = SpriteBatch;
        auto max_units = ShaderProgram::MaxTextureSlots();
        std::ostringstream ss;
        ss << "#version 330 core\n"
           //
           << "layout (location = 0) out vec4 color;\n"
           //
           << "uniform sampler2D " << SB::U_SAMPLER_ARRAY << "[" << max_units << "];\n"
           << "uniform float " << U_OUTLINE_DISTANCE << ";\n"
           << "uniform vec4 " << U_OUTLINE_COLOR << ";\n"
           //
           << "in vertex_attributes\n"
           << "{\n"
           << "  vec4 pos;\n"
           << "  vec2 uv;\n"
           << "  flat uint tid;\n"
           << "  vec4 color;\n"
           << "} v_va;\n"
           //
           << "const float s = 1.0 / 16.0;\n"  // smoothing ratio
           << "const float se_0 = 0.5 - s;\n"  // smoothing edge_0
           << "const float se_1 = 0.5 + s;\n"  // smoothing edge_1
           //
           << "void main()\n"
           << "{\n"
           << "  float d = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv).a;\n"
           << "  float olf = smoothstep(se_0, se_1, d);\n"
           << "  vec4 c = mix(" << U_OUTLINE_COLOR << ", v_va.color, olf);\n"
           //
           << "  float ole_0 = " << U_OUTLINE_DISTANCE << " - s;\n"
           << "  float ole_1 = " << U_OUTLINE_DISTANCE << " + s;\n"
           << "  float alpha = smoothstep(ole_0, ole_1, d);\n"
           //
           << "  color = vec4(c.rgb, c.a * alpha);\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

} // namespace rdge
