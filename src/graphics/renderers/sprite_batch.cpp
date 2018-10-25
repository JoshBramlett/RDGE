#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <sstream>

namespace rdge {

struct sprite_vertex
{
    math::vec3 pos;
    math::vec2 uv;
    uint32 tid;
    uint32 color;
};

namespace {

constexpr uint32 VERTEX_SIZE = sizeof(sprite_vertex);
constexpr uint32 SPRITE_SIZE = VERTEX_SIZE * 4;

} // anonymous namespace

SpriteBatch::SpriteBatch (uint16 capacity)
    : m_capacity(capacity)
{
    SDL_assert(capacity != 0);
    this->blend.enabled = true;

    // SpriteBatch implements it's buffer object streaming using the orphaning
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

    uint32 vbo_size  = static_cast<uint32>(m_capacity) * SPRITE_SIZE;
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VA_POS_INDEX);
    opengl::VertexAttribPointer(VA_POS_INDEX,
                                3,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, pos)));

    opengl::EnableVertexAttribute(VA_UV_INDEX);
    opengl::VertexAttribPointer(VA_UV_INDEX,
                                2,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, uv)));

    opengl::EnableVertexAttribute(VA_TID_INDEX);
    opengl::VertexAttribIPointer(VA_TID_INDEX, // Note the "I" - required for integer types
                                 1,
                                 GL_UNSIGNED_INT,
                                 VERTEX_SIZE,
                                 reinterpret_cast<void*>(offsetof(sprite_vertex, tid)));

    opengl::EnableVertexAttribute(VA_COLOR_INDEX);
    opengl::VertexAttribPointer(VA_COLOR_INDEX,
                                4,
                                GL_UNSIGNED_BYTE,
                                true,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, color)));

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

    m_shader.Build();

    // An identity matrix is placed at the base of the transform stack and used
    // as a no-op transform if none are provided.
    PushTransformation(math::mat4::identity(), true);

    DLOG() << "SpriteBatch[" << this << "]"
           << " capacity=" << m_capacity
           << " vao[" << m_vao << "]"
           << " vbo[" << m_vbo << "].size=" << vbo_size
           << " ibo[" << m_ibo << "].size=" << ibo_size;
}

SpriteBatch::~SpriteBatch (void) noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

SpriteBatch::SpriteBatch (SpriteBatch&& other) noexcept
    : blend(other.blend)
    , m_cursor(other.m_cursor)
    , m_submissions(other.m_submissions)
    , m_capacity(other.m_capacity)
    , m_combined(other.m_combined)
    , m_shader(std::move(other.m_shader))
    , m_transformStack(std::move(other.m_transformStack))
    , m_transform(other.m_transform)
{
    // Swap used so moved-from object dtor can cleanup
    std::swap(m_vao, other.m_vao);
    std::swap(m_vbo, other.m_vbo);
    std::swap(m_ibo, other.m_ibo);

    other.m_cursor = nullptr;
    other.m_submissions = 0;
    other.m_capacity = 0;
    other.m_transform = nullptr;
}

SpriteBatch&
SpriteBatch::operator= (SpriteBatch&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->blend = rhs.blend;
        m_cursor = rhs.m_cursor;
        m_submissions = rhs.m_submissions;
        m_capacity = rhs.m_capacity;
        m_combined = rhs.m_combined;
        m_shader = std::move(rhs.m_shader);
        m_transformStack = std::move(rhs.m_transformStack);
        m_transform = rhs.m_transform;

        std::swap(m_vao, rhs.m_vao);
        std::swap(m_vbo, rhs.m_vbo);
        std::swap(m_ibo, rhs.m_ibo);

        rhs.m_cursor = nullptr;
        rhs.m_submissions = 0;
        rhs.m_capacity = 0;
        rhs.m_transform = nullptr;
    }

    return *this;
}

uint16
SpriteBatch::Capacity (void) const noexcept
{
    return static_cast<uint16>(m_capacity);
}

void
SpriteBatch::SetView (const OrthographicCamera& camera)
{
    SDL_assert(m_vao != 0);
    m_combined = camera.combined;

    m_shader.shader.Enable();
    m_shader.shader.SetUniformValue(U_PROJ_XF, camera.combined);
    m_shader.shader.Disable();
}

void
SpriteBatch::Prime (void)
{
    m_shader.shader.Enable();

    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void* buffer = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    m_cursor = static_cast<sprite_vertex*>(buffer);
    m_submissions = 0;
}

void
SpriteBatch::Prime (SpriteBatchShader& shader)
{
    SDL_assert(m_vao != 0);

    shader.shader.Enable();
    shader.shader.SetUniformValue(U_PROJ_XF, m_combined);

    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void* buffer = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    m_cursor = static_cast<sprite_vertex*>(buffer);
    m_submissions = 0;
}

void
SpriteBatch::Draw (const sprite_data& sprite)
{
    const auto& p = sprite.pos;
    const auto& sz = sprite.size;
    auto c = static_cast<uint32>(sprite.color);

    m_cursor->pos   = math::vec3(p, sprite.depth);
    m_cursor->uv    = sprite.uvs[0];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor->pos.transform(*m_transform);
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x, p.y + sz.h, sprite.depth);
    m_cursor->uv    = sprite.uvs[1];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor->pos.transform(*m_transform);
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x + sz.w, p.y + sz.h, sprite.depth);
    m_cursor->uv    = sprite.uvs[2];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor->pos.transform(*m_transform);
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x + sz.w, p.y, sprite.depth);
    m_cursor->uv    = sprite.uvs[3];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor->pos.transform(*m_transform);
    m_cursor++;

    m_submissions++;
}

void
SpriteBatch::Flush (const std::vector<Texture>& textures)
{
    // Sanity check the same VBO is bound throughout the draw call
    SDL_assert(m_vbo == static_cast<uint32>(opengl::GetInt(GL_ARRAY_BUFFER_BINDING)));

    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    if (m_submissions > 0)
    {
        for (auto& texture : textures)
        {
            texture.Activate();
        }

        this->blend.Apply();
        opengl::BindVertexArray(m_vao);
        opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

        opengl::DrawElements(GL_TRIANGLES, (m_submissions * 6), GL_UNSIGNED_INT, nullptr);

        opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
        opengl::UnbindVertexArrays();
    }
}

void
SpriteBatch::PushTransformation (const math::mat4& matrix, bool override)
{
    if (override)
    {
        m_transformStack.push_back(matrix);
    }
    else
    {
        m_transformStack.push_back(m_transformStack.back() * matrix);
    }

    m_transform = &m_transformStack.back();
}

void
SpriteBatch::PopTransformation (void)
{
    if (m_transformStack.size() > 1)
    {
        m_transformStack.pop_back();
    }

    m_transform = &m_transformStack.back();
}

} // namespace rdge
