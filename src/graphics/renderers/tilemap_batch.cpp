#include <rdge/graphics/renderers/tilemap_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL_assert.h>
#include <GL/glew.h>

#include <sstream>

namespace rdge {

namespace {

struct tile_vertex
{
    math::vec3 pos;
    math::vec2 uv;
    uint32     color = 0xFFFFFFFF;
};

constexpr size_t VERTEX_SIZE = sizeof(tile_vertex);
constexpr size_t TILE_SIZE = VERTEX_SIZE * 4;

} // anonymous namespace

TilemapBatch::TilemapBatch (const SpriteSheet& sheet, float scale)
    : m_tilemap(sheet.tilemap)
    , m_texture(sheet.texture)
{
    SDL_assert(scale > 0.f);
    m_tilemap.tile_size *= scale;
    m_texture->unit_id = 0;

    std::ostringstream vert;
    vert << "#version 330 core\n"
         //
         << "layout (location = " << VATTR_POS_INDEX   << ") in vec4 v_pos;\n"
         << "layout (location = " << VATTR_UV_INDEX    << ") in vec2 v_uv;\n"
         << "layout (location = " << VATTR_COLOR_INDEX << ") in vec4 v_color;\n"
         //
         << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
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
         << "  gl_Position  = " << UNI_PROJ_MATRIX << " * v_pos;\n"
         << "}\n";

    std::ostringstream frag;
    frag << "#version 330 core\n"
         //
         << "layout (location = 0) out vec4 color;\n"
         //
         << "uniform sampler2D u_texture;\n"
         //
         << "in vertex_attributes\n"
         << "{\n"
         << "  vec2 uv;\n"
         << "  vec4 color;\n"
         << "} vertex;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  color = vertex.color * texture(u_texture, vertex.uv);\n"
         << "}\n";

    m_shader = Shader(vert.str(), frag.str());

    // TilemapBatch implements it's buffer object streaming using the orphaning
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

    uint32 vbo_size = m_tilemap.tile_count * TILE_SIZE;
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VATTR_POS_INDEX);
    opengl::VertexAttribPointer(VATTR_POS_INDEX,
                                3,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, pos)));

    opengl::EnableVertexAttribute(VATTR_UV_INDEX);
    opengl::VertexAttribPointer(VATTR_UV_INDEX,
                                2,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, uv)));

    opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);
    opengl::VertexAttribPointer(VATTR_COLOR_INDEX,
                                4,
                                GL_UNSIGNED_BYTE,
                                true,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(tile_vertex, color)));

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // IBO
    m_ibo = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    uint32 ibo_count = m_tilemap.tile_count * 6;
    uint32 ibo_size = ibo_count * sizeof(uint32);

    uint32* ibo_data;
    if (UNLIKELY(!RDGE_MALLOC(ibo_data, ibo_size, nullptr)))
    {
        RDGE_THROW("Failed to allocate memory");
    }

    for (uint32 i = 0, idx = 0, offset = 0;
         i < m_tilemap.tile_count;
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
    RDGE_FREE(ibo_data, nullptr);

    opengl::UnbindVertexArrays();

    OrthographicCamera camera;
    SetView(camera);

    DLOG() << "TilemapBatch[" << this << "]"
           << " capacity=" << m_tilemap.tile_size
           << " vao[" << m_vao << "]"
           << " vbo[" << m_vbo << "].size=" << vbo_size
           << " ibo[" << m_ibo << "].size=" << ibo_size;
}

TilemapBatch::~TilemapBatch (void) noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

TilemapBatch::TilemapBatch (TilemapBatch&& other) noexcept
    : m_shader(std::move(other.m_shader))
    , m_bounds(other.m_bounds)
    , m_far(other.m_far)
    , m_tilemap(std::move(other.m_tilemap))
    , m_texture(std::move(other.m_texture))
{
    std::swap(m_vao, other.m_vao);
    std::swap(m_vbo, other.m_vbo);
    std::swap(m_ibo, other.m_ibo);
}

TilemapBatch&
TilemapBatch::operator= (TilemapBatch&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_shader = std::move(rhs.m_shader);
        m_bounds = rhs.m_bounds;
        m_far = rhs.m_far;
        m_tilemap = std::move(rhs.m_tilemap);
        m_texture = std::move(rhs.m_texture);

        std::swap(m_vao, rhs.m_vao);
        std::swap(m_vbo, rhs.m_vbo);
        std::swap(m_ibo, rhs.m_ibo);
    }

    return *this;
}

void
TilemapBatch::SetView (const OrthographicCamera& camera)
{
    SDL_assert(m_vao != 0);

    m_shader.Enable();
    m_shader.SetUniformValue(UNI_PROJ_MATRIX, camera.combined);
    m_shader.Disable();

    float width = camera.viewport_size.w * camera.zoom;
    float height = camera.viewport_size.h * camera.zoom;
    float w = width * math::abs(camera.up.y) + height * math::abs(camera.up.x);
    float h = height * math::abs(camera.up.y) + width * math::abs(camera.up.x);

    math::vec2 origin(camera.position.x - (w * 0.5f), camera.position.y - (h * 0.5f));
    m_bounds = physics::aabb(origin, w, h);
    m_far = -camera.far;
}

void
TilemapBatch::Draw (void)
{
    SDL_assert(m_vao != 0);

    m_shader.Enable();
    m_texture->Activate();

    const auto& sz = m_tilemap.tile_size;
    for (size_t i = 0; i < m_tilemap.layer_count; i++)
    {
        size_t submissions = 0;

        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        auto cursor = static_cast<tile_vertex*>(opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

        auto& layer = m_tilemap.layers[i];
        for (size_t j = 0; j < m_tilemap.tile_count; j++)
        {
            auto& tile = layer.tiles[j];
            if (tile.index != tilemap_data::INVALID_TILE)
            {
                float x = (tile.location.x * sz.x);
                float y = -(tile.location.y * sz.y);

                cursor->pos   = math::vec3(x, y, m_far);
                cursor->uv    = tile.coords[0];
                cursor->color = 0xFFFFFFFF;
                cursor++;

                cursor->pos   = math::vec3(x, y + sz.y, m_far);
                cursor->uv    = tile.coords[1];
                cursor->color = 0xFFFFFFFF;
                cursor++;

                cursor->pos   = math::vec3(x + sz.x, y + sz.y, m_far);
                cursor->uv    = tile.coords[2];
                cursor->color = 0xFFFFFFFF;
                cursor++;

                cursor->pos   = math::vec3(x + sz.x, y, m_far);
                cursor->uv    = tile.coords[3];
                cursor->color = 0xFFFFFFFF;
                cursor++;

                submissions++;
            }
        }

        opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
        opengl::UnbindBuffers(GL_ARRAY_BUFFER);

        //this->blend.Apply();

        opengl::BindVertexArray(m_vao);
        opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

        opengl::DrawElements(GL_TRIANGLES, (submissions * 6), GL_UNSIGNED_INT, nullptr);

        opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
        opengl::UnbindVertexArrays();
    }
}

} // namespace rdge
