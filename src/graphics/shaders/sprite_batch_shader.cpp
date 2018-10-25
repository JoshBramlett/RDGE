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
    this->shader.SetUniformValue(SpriteBatch::U_SAMPLER_ARRAY,
                                 static_cast<uint32>(slots.size()),
                                 slots.data());
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

// A note on distance field shaders (from libgdx):
// https://github.com/libgdx/libgdx/wiki/Distance-field-fonts
//
// Remember that distance is a value between 0 and 1, with 0 being far away from
// the letter, 0.5 being right on the edge, and 1 being well inside it. The
// smoothstep function in the shader above is mapping values well below 0.5 to 0,
// and values well above 0.5 to 1, but gives a smooth transition around 0.5 to
// provide antialiasing. The softness of this transition is configured by the
// smoothing constant, which you should tweak to be correct for your font and scale.

// The right smoothing value for crisp fonts is 0.25f / (spread * scale), where
// spread is the value you used when generating the font, and scale is the scale
// you're drawing it at (how pixels in the distance field font are mapped to screen
// pixels). If the scale is not constant, you can pass it in via a uniform variable.

namespace {

constexpr float
CalculateSmoothing (float spread, float scale)
{
    return (0.25f / (spread * scale));
}

} // anonymous namespace

DistanceFieldShader::DistanceFieldShader (float spread, float scale)
    : m_spread(spread)
    , m_scale(scale)
{ }

void
DistanceFieldShader::Build (void)
{
    SDL_assert(m_spread > 0.f);
    SDL_assert(m_scale > 0.f);

    SpriteBatchShader::Build();

    this->shader.Enable();
    this->shader.SetUniformValue(U_SMOOTHING, CalculateSmoothing(m_spread, m_scale));
    this->shader.Disable();
}

void
DistanceFieldShader::SetSpread (float spread)
{
    m_spread = spread;
    if (!this->shader.IsEmpty())
    {
        SDL_assert(m_spread > 0.f);
        SDL_assert(m_scale > 0.f);

        this->shader.Enable();
        this->shader.SetUniformValue(U_SMOOTHING, CalculateSmoothing(m_spread, m_scale));
        this->shader.Disable();
    }
}

void
DistanceFieldShader::SetScale (float scale)
{
    m_scale = scale;
    if (!this->shader.IsEmpty())
    {
        SDL_assert(m_spread > 0.f);
        SDL_assert(m_scale > 0.f);

        this->shader.Enable();
        this->shader.SetUniformValue(U_SMOOTHING, CalculateSmoothing(m_spread, m_scale));
        this->shader.Disable();
    }
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
           << "uniform float " << U_SMOOTHING << ";\n"
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
           << "  float d = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv).a;\n"
           //
           << "  float se_0 = 0.5 - " << U_SMOOTHING << ";\n"
           << "  float se_1 = 0.5 + " << U_SMOOTHING << ";\n"
           << "  float alpha = smoothstep(se_0, se_1, d);\n"
           << "  color = vec4(v_va.color.rgb, v_va.color.a * alpha);\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

DistanceFieldOutlineShader::DistanceFieldOutlineShader (float spread,
                                                        float scale,
                                                        const color& c,
                                                        float d)
    : DistanceFieldShader(spread, scale)
    , m_color(static_cast<math::vec4>(c))
    , m_distance(math::clamp(d, MIN_DISTANCE, MAX_DISTANCE))
{ }

void
DistanceFieldOutlineShader::Build (void)
{
    DistanceFieldShader::Build();

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
           << "uniform float " << U_SMOOTHING << ";\n"
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
           << "void main()\n"
           << "{\n"
           << "  float d = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv).a;\n"
           << "  float se_0 = 0.5 - " << U_SMOOTHING << ";\n"
           << "  float se_1 = 0.5 + " << U_SMOOTHING << ";\n"
           << "  float olf = smoothstep(se_0, se_1, d);\n"
           << "  vec4 c = mix(" << U_OUTLINE_COLOR << ", v_va.color, olf);\n"
           //
           << "  float ole_0 = " << U_OUTLINE_DISTANCE << " - " << U_SMOOTHING << ";\n"
           << "  float ole_1 = " << U_OUTLINE_DISTANCE << " + " << U_SMOOTHING << ";\n"
           << "  float alpha = smoothstep(ole_0, ole_1, d);\n"
           //
           << "  color = vec4(c.rgb, c.a * alpha);\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

DistanceFieldDropShadowShader::DistanceFieldDropShadowShader (float spread,
                                                              float scale,
                                                              const color& c,
                                                              float s,
                                                              const math::vec2& offset)
    : DistanceFieldShader(spread, scale)
    , m_color(static_cast<math::vec4>(c))
    , m_smoothing(math::clamp(s, MIN_SMOOTHING, MAX_SMOOTHING))
    , m_offset(offset)
{ }

void
DistanceFieldDropShadowShader::Build (void)
{
    DistanceFieldShader::Build();

    this->shader.Enable();
    this->shader.SetUniformValue(U_SHADOW_COLOR, m_color);
    this->shader.SetUniformValue(U_SHADOW_SMOOTHING, m_smoothing);
    this->shader.SetUniformValue(U_SHADOW_OFFSET, m_offset);
    this->shader.Disable();
}

void
DistanceFieldDropShadowShader::SetShadowColor (const color& c)
{
    m_color = static_cast<math::vec4>(c);
    if (!this->shader.IsEmpty())
    {
        this->shader.Enable();
        this->shader.SetUniformValue(U_SHADOW_COLOR, m_color);
        this->shader.Disable();
    }
}

void
DistanceFieldDropShadowShader::SetShadowSmoothing (float s)
{
    m_smoothing = math::clamp(s, MIN_SMOOTHING, MAX_SMOOTHING);
    if (!this->shader.IsEmpty())
    {
        this->shader.Enable();
        this->shader.SetUniformValue(U_SHADOW_SMOOTHING, m_smoothing);
        this->shader.Disable();
    }
}

void
DistanceFieldDropShadowShader::SetShadowOffset (const math::vec2& offset)
{
    m_offset = offset;
    if (!this->shader.IsEmpty())
    {
        this->shader.Enable();
        this->shader.SetUniformValue(U_SHADOW_OFFSET, m_offset);
        this->shader.Disable();
    }
}

const std::string&
DistanceFieldDropShadowShader::GetFragmentSource (void)
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
           << "uniform float " << U_SMOOTHING << ";\n"
           << "uniform vec4 " << U_SHADOW_COLOR << ";\n"
           << "uniform float " << U_SHADOW_SMOOTHING << ";\n"
           << "uniform vec2 " << U_SHADOW_OFFSET << ";\n"
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
           << "  float d = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], v_va.uv).a;\n"
           << "  float se_0 = 0.5 - " << U_SMOOTHING << ";\n"
           << "  float se_1 = 0.5 + " << U_SMOOTHING << ";\n"
           << "  float text_alpha = smoothstep(se_0, se_1, d);\n"
           << "  vec4 text_color = vec4(v_va.color.rgb, v_va.color.a * text_alpha);\n"
           //
           << "  vec2 stexel = vec2(v_va.uv - " << U_SHADOW_OFFSET << ");\n"
           << "  float sd = texture(" << SB::U_SAMPLER_ARRAY << "[v_va.tid], stexel).a;\n"
           //
           << "  float sse_0 = 0.5 - " << U_SHADOW_SMOOTHING << ";\n"
           << "  float sse_1 = 0.5 + " << U_SHADOW_SMOOTHING << ";\n"
           << "  float shadow_alpha = smoothstep(sse_0, sse_1, sd);\n"
           //
           << "  vec4 shadow_color = vec4(" << U_SHADOW_COLOR << ".rgb, " << U_SHADOW_COLOR << ".a * shadow_alpha);\n"
           << "  color = mix(shadow_color, text_color, text_color.a);\n"
           << "}\n";

        f_src = ss.str();
    }

    return f_src;
}

} // namespace rdge
