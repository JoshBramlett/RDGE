#include <rdge/graphics/renderers/tile_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/debug/assert.hpp>

#include <GL/glew.h>

#include <sstream>

namespace rdge {

struct tile_vertex
{
    math::vec3 pos;
    math::vec2 uv;
    uint32     color = 0xFFFFFFFF;
};

namespace {

constexpr size_t VERTEX_SIZE = sizeof(tile_vertex);
constexpr size_t TILE_SIZE = VERTEX_SIZE * 4;

} // anonymous namespace

TileBatch::TileBatch (uint16 capacity, const math::vec2& tile_size)
    : m_capacity(capacity)
    , m_tileSize(tile_size)
{
    this->blend.enabled = true;

    std::ostringstream vert;
    vert << "#version 330 core\n"
         //
         << "layout (location = " << VA_POS_INDEX   << ") in vec4 v_pos;\n"
         << "layout (location = " << VA_UV_INDEX    << ") in vec2 v_uv;\n"
         << "layout (location = " << VA_COLOR_INDEX << ") in vec4 v_color;\n"
         //
         << "uniform mat4 " << U_PROJ_XF << ";\n"
         //
         << "out vertex_attributes\n"
         << "{\n"
         << "  vec2 uv;\n"
         << "  vec4 color;\n"
         << "} vertex;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  vertex.uv    = v_uv;\n"
         << "  vertex.color = v_color;\n"
         << "  gl_Position  = " << U_PROJ_XF << " * v_pos;\n"
         << "}\n";

    std::ostringstream frag;
    frag << "#version 330 core\n"
         //
         << "layout (location = 0) out vec4 color;\n"
         //
         << "uniform sampler2D " << U_SAMPLER << ";\n"
         //
         << "in vertex_attributes\n"
         << "{\n"
         << "  vec2 uv;\n"
         << "  vec4 color;\n"
         << "} vertex;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  color = vertex.color * texture(" << U_SAMPLER << ", vertex.uv);\n"
         << "}\n";

    m_shader = ShaderProgram(vert.str(), frag.str());

    // TileBatch implements it's buffer object streaming using the orphaning
    // technique, which means for every frame we ask OpenGL to give us a new buffer.
    // Since we request the same size, it's likely (though not guaranteed) that
    // no allocation will take place.  The major drawback of this approach is
    // is it's dependent on the implementation, so performance may differ.
    // https://www.opengl.org/wiki/Buffer_Object_Streaming#Buffer_re-specification
    m_vao = opengl::CreateVertexArray();
    opengl::BindVertexArray(m_vao);

    // VBO
    m_vbo = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

    uint32 vbo_size = m_capacity * TILE_SIZE;
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VA_POS_INDEX);
    opengl::VertexAttribPointer(VA_POS_INDEX,
                                3,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, pos)));

    opengl::EnableVertexAttribute(VA_UV_INDEX);
    opengl::VertexAttribPointer(VA_UV_INDEX,
                                2,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, uv)));

    opengl::EnableVertexAttribute(VA_COLOR_INDEX);
    opengl::VertexAttribPointer(VA_COLOR_INDEX,
                                4,
                                GL_UNSIGNED_BYTE,
                                true,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, color)));

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // IBO
    m_ibo = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    size_t ibo_count = m_capacity * 6;
    size_t ibo_size = ibo_count * sizeof(uint32);

    uint32* ibo_data = (uint32*)RDGE_MALLOC(ibo_size, memory_bucket_graphics);
    if (RDGE_UNLIKELY(!ibo_data))
    {
        RDGE_THROW("Memory allocation failed");
    }

    for (uint32 i = 0, idx = 0, offset = 0;
         i < m_capacity;
         i++, idx = i * 6, offset = i * 4)
    {
        ibo_data[idx]     = offset;
        ibo_data[idx + 1] = offset + 1;
        ibo_data[idx + 2] = offset + 2;

        ibo_data[idx + 3] = offset + 2;
        ibo_data[idx + 4] = offset + 3;
        ibo_data[idx + 5] = offset;
    }

    opengl::SetBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_size, ibo_data, GL_STATIC_DRAW);
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    RDGE_FREE(ibo_data, memory_bucket_graphics);

    opengl::UnbindVertexArrays();

    DLOG() << "TileBatch[" << this << "]"
           << " capacity=" << m_capacity
           << " tile_size=" << m_tileSize
           << " vao[" << m_vao << "]"
           << " vbo[" << m_vbo << "].size=" << vbo_size
           << " ibo[" << m_ibo << "].size=" << ibo_size;
}

TileBatch::~TileBatch (void) noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

TileBatch::TileBatch (TileBatch&& other) noexcept
    : blend(other.blend)
    , depth(other.depth)
    , m_cursor(other.m_cursor)
    , m_submissions(other.m_submissions)
    , m_capacity(other.m_capacity)
    , m_combined(other.m_combined)
    , m_shader(std::move(other.m_shader))
    , m_tileSize(other.m_tileSize)
{
    std::swap(m_vao, other.m_vao);
    std::swap(m_vbo, other.m_vbo);
    std::swap(m_ibo, other.m_ibo);
}

TileBatch&
TileBatch::operator= (TileBatch&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->blend = rhs.blend;
        this->depth = rhs.depth;
        m_cursor = rhs.m_cursor;
        m_submissions = rhs.m_submissions;
        m_capacity = rhs.m_capacity;
        m_combined = rhs.m_combined;
        m_shader = std::move(rhs.m_shader);
        m_tileSize = rhs.m_tileSize;

        std::swap(m_vao, rhs.m_vao);
        std::swap(m_vbo, rhs.m_vbo);
        std::swap(m_ibo, rhs.m_ibo);

        rhs.m_cursor = nullptr;
        rhs.m_capacity = 0;
    }

    return *this;
}

void
TileBatch::SetView (const OrthographicCamera& camera, float depth)
{
    RDGE_ASSERT(m_vao != 0);
    m_combined = camera.combined;
    this->depth = (!std::isnan(depth)) ? depth : -camera.far;

    m_shader.Enable();
    m_shader.SetUniformValue(U_PROJ_XF, camera.combined);
    m_shader.Disable();
}

void
TileBatch::Prime (void)
{
    RDGE_ASSERT(m_vao != 0);

    m_shader.Enable();
    m_shader.SetUniformValue(U_SAMPLER, TEXTURE_UNIT_ID);

    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void* buffer = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    m_cursor = static_cast<tile_vertex*>(buffer);
    m_submissions = 0;
}

void
TileBatch::Draw (const tile_cell_chunk& chunk, color c)
{
    RDGE_ASSERT(m_vao != 0);
    RDGE_ASSERT(m_vbo == static_cast<uint32>(opengl::GetInt(GL_ARRAY_BUFFER_BINDING)));
    RDGE_ASSERT(m_submissions <= m_capacity);

    const auto& sz = m_tileSize;
    auto ic = static_cast<uint32>(c);

    for (size_t i = 0; i < chunk.cell_count; i++)
    {
        auto& cell = chunk.cells[i];
        if (cell.uvs)
        {
            RDGE_ASSERT(*cell.uvs);
            RDGE_ASSERT(!(*cell.uvs)->is_empty());
            m_cursor->pos   = math::vec3(cell.pos, this->depth);
            m_cursor->uv    = (**cell.uvs)[0];
            m_cursor->color = ic;
            m_cursor++;

            m_cursor->pos   = math::vec3(cell.pos.x, cell.pos.y + sz.y, this->depth);
            m_cursor->uv    = (**cell.uvs)[1];
            m_cursor->color = ic;
            m_cursor++;

            m_cursor->pos   = math::vec3(cell.pos.x + sz.x, cell.pos.y + sz.y, this->depth);
            m_cursor->uv    = (**cell.uvs)[2];
            m_cursor->color = ic;
            m_cursor++;

            m_cursor->pos   = math::vec3(cell.pos.x + sz.x, cell.pos.y, this->depth);
            m_cursor->uv    = (**cell.uvs)[3];
            m_cursor->color = ic;
            m_cursor++;

            m_submissions++;
        }
    }
}

void
TileBatch::Flush (const Texture& texture)
{
    // Sanity check the same VBO is bound throughout the draw call
    RDGE_ASSERT(m_vbo == static_cast<uint32>(opengl::GetInt(GL_ARRAY_BUFFER_BINDING)));

    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    if (m_submissions > 0)
    {
        texture.Activate();
        this->blend.Apply();

        opengl::BindVertexArray(m_vao);
        opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

        opengl::DrawElements(GL_TRIANGLES, (m_submissions * 6), GL_UNSIGNED_INT, nullptr);

        opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
        opengl::UnbindVertexArrays();
    }
}

} // namespace rdge
