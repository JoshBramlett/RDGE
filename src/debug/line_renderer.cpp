#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/intrinsics.hpp>

#include <rdge/internal/hints.hpp>
#include <rdge/internal/logger_macros.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>

#include <stdlib.h> // malloc
#include <utility> // std::pair
#include <sstream>
#include <memory>

namespace {

using namespace rdge;
using namespace rdge::debug;

struct line_vertex
{
    math::vec3 pos;
    uint32     color = 0xFFFFFFFF;
};

constexpr uint32 VERTEX_SIZE = sizeof(line_vertex);
constexpr uint32 LINE_SIZE = VERTEX_SIZE * 2;

constexpr uint32 VATTR_POS_INDEX = 0;
constexpr uint32 VATTR_COLOR_INDEX = 1;
constexpr const char* UNI_PROJ_MATRIX = "proj_matrix";

std::pair<std::string, std::string>
DefaultShaderSource (void)
{
    std::ostringstream vert;
    vert << "#version 330 core\n"
         //
         << "layout (location = " << VATTR_POS_INDEX   << ") in vec4 position;\n"
         << "layout (location = " << VATTR_COLOR_INDEX << ") in vec4 color;\n"
         //
         << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
         //
         << "out vec4 vertex_color;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  vertex_color = color;\n"
         << "  gl_Position  = " << UNI_PROJ_MATRIX << " * position;\n"
         << "}\n";

    std::ostringstream frag;
    frag << "#version 330 core\n"
         //
         << "layout (location = 0) out vec4 color;\n"
         //
         << "in vec4 vertex_color;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  color = vertex_color;\n"
         << "}\n";

    return std::make_pair(vert.str(), frag.str());
}

// NOTE LineRenderer draws from stateless functions, which is why the definition
//      is hidden.  The reason it's implemented as a singleton is because it
//      requires a valid OpenGL context so we need to do lazy instantiation.
class LineRenderer
{
public:
    explicit LineRenderer (uint16 capacity);
    ~LineRenderer (void) noexcept;

    LineRenderer (const LineRenderer&) = delete;
    LineRenderer& operator= (const LineRenderer&) = delete;
    LineRenderer (LineRenderer&&) = delete;
    LineRenderer& operator= (LineRenderer&&) = delete;

    void SetProjection (const math::mat4& projection);
    void DrawLine (const math::vec3& pa, const math::vec3& pb) noexcept;
    void DrawLine (const math::vec3& pa, const math::vec3& pb, uint32 color) noexcept;

    void Flush (void);

    uint32 default_color = static_cast<uint32>(color::YELLOW);

private:
    uint32 m_vao = 0;
    uint32 m_vbo = 0;

    line_vertex* m_buffer = nullptr;
    uint32       m_submissions = 0;
    uint32       m_capacity = 0;

    std::unique_ptr<Shader> m_shader;
    math::mat4              m_projection;
};

LineRenderer::LineRenderer (uint16 capacity)
    : m_capacity(capacity)
{
    SDL_assert(capacity != 0);
    SDL_assert(SDL_GL_GetCurrentContext() != nullptr);

    auto source = DefaultShaderSource();
    m_shader = std::make_unique<Shader>(source.first, source.second);
    if (UNLIKELY(m_shader == nullptr))
    {
        RDGE_THROW("Memory allocation failed");
    }

    m_vao = opengl::CreateVertexArray();
    opengl::BindVertexArray(m_vao);

    m_vbo = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

    uint32 vbo_size = static_cast<uint32>(m_capacity) * LINE_SIZE;
    m_buffer = reinterpret_cast<line_vertex*>(malloc(vbo_size));
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VATTR_POS_INDEX);
    opengl::VertexAttribPointer(VATTR_POS_INDEX,
                                3,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(line_vertex, pos)));

    opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);
    opengl::VertexAttribPointer(VATTR_COLOR_INDEX,
                                4,
                                GL_UNSIGNED_BYTE,
                                true,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(line_vertex, color)));

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    auto viewport = opengl::GetViewport();
    auto width  = viewport[2] / 2.f;
    auto height = viewport[3] / 2.f;
    m_projection = math::mat4::orthographic(-width, width, -height, height, 0.f, 100.f);

    m_shader->Enable();
    m_shader->SetUniformValue(UNI_PROJ_MATRIX, m_projection);
    m_shader->Disable();

    std::ostringstream ss;
    ss << "debug::LineRenderer"
       << " capacity=" << m_capacity
       << " vao[" << m_vao << "]"
       << " vbo[" << m_vbo << "].size=" << vbo_size;
    DLOG(ss.str());
}

LineRenderer::~LineRenderer (void) noexcept
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void
LineRenderer::DrawLine (const math::vec3& pa, const math::vec3& pb) noexcept
{
    DrawLine(pa, pb, this->default_color);
}

void
LineRenderer::DrawLine (const math::vec3& pa, const math::vec3& pb, uint32 color) noexcept
{
    SDL_assert(m_submissions <= m_capacity);

    line_vertex* cursor = m_buffer + m_submissions;

    cursor->pos = pa;
    cursor->color = color;
    cursor++;

    cursor->pos = pb;
    cursor->color = color;

    m_submissions += 2;
}

void
LineRenderer::Flush (void)
{
    // because debug drawing is stateless, bail early for no submissions
    if (m_submissions == 0)
    {
        return;
    }

    m_shader->Enable();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

    void* mapped = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(mapped, m_buffer, LINE_SIZE * m_submissions);

    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    opengl::BindVertexArray(m_vao);
    opengl::DrawArrays(GL_LINES, 0, m_submissions);
    opengl::UnbindVertexArrays();

    m_submissions = 0;
}

void
LineRenderer::SetProjection (const math::mat4& projection)
{
    m_projection = projection;

    m_shader->Enable();
    m_shader->SetUniformValue(UNI_PROJ_MATRIX, m_projection);
    m_shader->Disable();
}

#ifdef RDGE_DEBUG
LineRenderer& Instance (uint16 cap = 5000)
{
    static LineRenderer instance(cap);
    return instance;
}
#endif

} // anonymous namespace

#ifdef RDGE_DEBUG

namespace rdge {
namespace debug {

void
InitializeRenderer (uint16 capacity)
{
    Instance(capacity);
}

void
FlushRenderer (void)
{
    auto& renderer = Instance();
    renderer.Flush();
}

void
SetLineWidth (float width)
{
    auto supported = opengl::GetSupportedLineWidth();
    if (width < supported[0] || width > supported[1])
    {
        std::ostringstream ss;
        ss << "SetLineWidth request is outside supported bounds and will be clamped."
           << "  min=" << supported[0]
           << "  max=" << supported[1]
           << "  requested=" << width;
        WLOG(ss.str());

        width = math::clamp(width, supported[0], supported[1]);
    }

    opengl::SetLineWidth(width);
}

void
SetLineColor (const color& c)
{
    auto& renderer = Instance();
    renderer.default_color = static_cast<uint32>(c);
}

void
SetProjection (const math::mat4& projection)
{
    auto& renderer = Instance();
    renderer.SetProjection(projection);
}

void
DrawLine (const math::vec3& pa, const math::vec3& pb)
{
    auto& renderer = Instance();
    renderer.DrawLine(pa, pb);
}

void
DrawLine (const math::vec3& pa, const math::vec3& pb, const color& c)
{
    auto& renderer = Instance();
    renderer.DrawLine(pa, pb, static_cast<uint32>(c));
}

void
DrawWireFrame (const SpriteVertices& vertices)
{
    auto& renderer = Instance();
    renderer.DrawLine(vertices[0].pos, vertices[1].pos);
    renderer.DrawLine(vertices[1].pos, vertices[2].pos);
    renderer.DrawLine(vertices[2].pos, vertices[3].pos);
    renderer.DrawLine(vertices[3].pos, vertices[0].pos);
}

void
DrawWireFrame (const SpriteVertices& vertices, const color& c)
{
    auto& renderer = Instance();
    auto ic = static_cast<uint32>(c);
    renderer.DrawLine(vertices[0].pos, vertices[1].pos, ic);
    renderer.DrawLine(vertices[1].pos, vertices[2].pos, ic);
    renderer.DrawLine(vertices[2].pos, vertices[3].pos, ic);
    renderer.DrawLine(vertices[3].pos, vertices[0].pos, ic);
}

} // namespace debug
} // namespace rdge

#endif
