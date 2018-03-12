#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <algorithm> // std::generate
#include <utility> // std::pair
#include <sstream>

namespace {

using namespace rdge;

constexpr uint32 VERTEX_SIZE = sizeof(sprite_vertex);
constexpr uint32 SPRITE_SIZE = VERTEX_SIZE * 4;

} // anonymous namespace

namespace rdge {

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

    m_shader = Shader(SpriteBatch::DefaultShader(ShaderType::VERTEX),
                      SpriteBatch::DefaultShader(ShaderType::FRAGMENT));

    // A requirement we impose on the fragment shader is to define a sampler2D array
    // with the element count equal to the maximum texture units available.  A vector
    // is filled with values { 0 ... MAX-1 } and set to the uniform.
    std::vector<int32> texture_units(Shader::MaxFragmentShaderUnits());
    int32 n = 0;
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    m_shader.Enable();
    m_shader.SetUniformValue(U_SAMPLER_ARRAY, texture_units.size(), texture_units.data());
    m_shader.Disable();

    // Create and register a single white pixel texture that will be associated with
    // all sprites that do not have a texture (i.e. color only).  In the fragment
    // shader the color is multiplied by the texture color to provide the final color,
    // so this "dummy" texture will be a no-op.  This avoids having conditional logic
    // in our shader code to determine whether the sprite has an associated texture.
    // Non-uniform flow control has a major negative performance impact.
    // https://www.opengl.org/wiki/Sampler_(GLSL)#Non-uniform_flow_control
    uint32 pixel = 0xFFFFFFFF;
    // TODO SDL 2.0.5 has a way to supply the pixel format
    auto dummy = SDL_CreateRGBSurfaceFrom(static_cast<void*>(&pixel), 1, 1, 32, 4, 0, 0, 0, 0);
    RegisterTexture(std::make_shared<Texture>(Surface(dummy))); // Surface temp will free 'dummy'

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
    , m_shader(std::move(other.m_shader))
    , m_transformStack(std::move(other.m_transformStack))
    , m_transform(other.m_transform)
    , m_textures(std::move(other.m_textures))
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
        m_shader = std::move(rhs.m_shader);
        m_transformStack = std::move(rhs.m_transformStack);
        m_transform = rhs.m_transform;
        m_textures = std::move(rhs.m_textures);

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
SpriteBatch::RegisterTexture (std::shared_ptr<Texture> texture)
{
    // no texture or already assigned a unit_id
    if (!texture || texture->unit_id != Texture::INVALID_UNIT_ID)
    {
        return;
    }

    auto size = static_cast<uint32>(m_textures.size());
    if (size >= (Shader::MaxFragmentShaderUnits() - 1))
    {
        // Check is for MAX-1 to accommodate for our default texture
        RDGE_THROW("Unable to register texture.  Max limit of " +
                   std::to_string(Shader::MaxFragmentShaderUnits()) +
                   " already reached.");
    }

    texture->unit_id = size;
    m_textures.emplace_back(texture);

    DLOG() << "SpriteBatch[" << this << "] texture registered:"
           << " texture=" << texture.get()
           << " unit_id=" << texture->unit_id;
}

void
SpriteBatch::SetView (const OrthographicCamera& camera)
{
    SDL_assert(m_vao != 0);

    m_shader.Enable();
    m_shader.SetUniformValue(U_PROJ_XF, camera.combined);
    m_shader.Disable();
}

void
SpriteBatch::PrepSubmit (void)
{
    m_shader.Enable();

    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void* buffer = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    m_cursor = static_cast<sprite_vertex*>(buffer);
    m_submissions = 0;
}

void
SpriteBatch::Prime (const OrthographicCamera& camera, Shader& shader)
{
    SDL_assert(m_vao != 0);

    shader.Enable();
    shader.SetUniformValue(U_PROJ_XF, camera.combined);

    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    void* buffer = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    m_cursor = static_cast<sprite_vertex*>(buffer);
    m_submissions = 0;
}

void
SpriteBatch::Submit (const SpriteVertices& vertices)
{
    SDL_assert(m_vbo == static_cast<uint32>(opengl::GetInt(GL_ARRAY_BUFFER_BINDING)));
    SDL_assert(m_submissions <= m_capacity);

    for (const auto& vertex : vertices)
    {
        m_cursor->pos   = vertex.pos.transform(*m_transform);
        m_cursor->uv    = vertex.uv;
        m_cursor->tid   = (vertex.tid == Texture::INVALID_UNIT_ID) ? 0 : vertex.tid;
        m_cursor->color = vertex.color;
        m_cursor++;
    }

    m_submissions++;
}

void
SpriteBatch::Submit (const sprite_data& sprite)
{
    const auto& p = sprite.pos;
    const auto& sz = sprite.size;
    auto c = static_cast<uint32>(sprite.color);

    m_cursor->pos   = math::vec3(p, sprite.depth);
    m_cursor->uv    = sprite.uvs[0];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x, p.y + sz.h, sprite.depth);
    m_cursor->uv    = sprite.uvs[1];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x + sz.w, p.y + sz.h, sprite.depth);
    m_cursor->uv    = sprite.uvs[2];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor++;

    m_cursor->pos   = math::vec3(p.x + sz.w, p.y, sprite.depth);
    m_cursor->uv    = sprite.uvs[3];
    m_cursor->tid   = sprite.tid;
    m_cursor->color = c;
    m_cursor++;

    m_submissions++;
}

void
SpriteBatch::Flush (void)
{
    // Sanity check our VBO is bound ensures noone else is binding a different VBO
    // during our submission process.
    SDL_assert(m_vbo == static_cast<uint32>(opengl::GetInt(GL_ARRAY_BUFFER_BINDING)));
    SDL_assert(m_submissions != 0);

    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // TODO Textures don't have to be activated for every draw call unless they have
    //      changed.  One alternative could be to create a container that manages
    //      all textures across all render targets.  I need to profile to determine
    //      how much of a cost enabling and activating a texture has.
    //      Keep in mind a limiting factor is that all textures must be assigned a
    //      unit id prior to activating.
    //
    //      Update: keep in mind multiple renderers will be used, so to work without
    //              a hitch the ease route would be to activate  the textures every
    //              draw call.  However, there's got to be a better way...base class
    //              that holds a static that keeps track of the last used texture
    //              and only activates if changed?
    for (auto& texture : m_textures)
    {
        texture->Activate();
    }

    this->blend.Apply();

    opengl::BindVertexArray(m_vao);
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    opengl::DrawElements(GL_TRIANGLES, (m_submissions * 6), GL_UNSIGNED_INT, nullptr);

    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    opengl::UnbindVertexArrays();
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

/* static */ const std::string&
SpriteBatch::DefaultShader (ShaderType type)
{
    if (type == ShaderType::VERTEX)
    {
        static std::string v_src;
        if (v_src.empty())
        {
            std::ostringstream vert;
            vert << "#version 330 core\n"
                 //
                 << "layout (location = " << VA_POS_INDEX   << ") in vec4 position;\n"
                 << "layout (location = " << VA_UV_INDEX    << ") in vec2 uv;\n"
                 << "layout (location = " << VA_TID_INDEX   << ") in uint tid;\n"
                 << "layout (location = " << VA_COLOR_INDEX << ") in vec4 color;\n"
                 //
                 << "uniform mat4 " << U_PROJ_XF << ";\n"
                 //
                 << "out vertex_attributes\n"
                 << "{\n"
                 << "  vec4 pos;\n"
                 << "  vec2 uv;\n"
                 << "  flat uint tid;\n"
                 << "  vec4 color;\n"
                 << "} vertex;\n"
                 //
                 << "void main()\n"
                 << "{\n"
                 << "  vertex.pos   = position;\n"
                 << "  vertex.uv    = uv;\n"
                 << "  vertex.tid   = tid;\n"
                 << "  vertex.color = color;\n"
                 << "  gl_Position  = " << U_PROJ_XF << " * position;\n"
                 << "}\n";

            v_src = vert.str();
        }

        return v_src;
    }
    else if (type == ShaderType::FRAGMENT)
    {
        static std::string f_src;
        if (f_src.empty())
        {
            std::ostringstream frag;
            frag << "#version 330 core\n"
                 //
                 << "layout (location = 0) out vec4 color;\n"
                 //
                 << "uniform sampler2D "
                    << U_SAMPLER_ARRAY << "[" << Shader::MaxFragmentShaderUnits() << "];\n"
                 //
                 << "in vertex_attributes\n"
                 << "{\n"
                 << "  vec4 pos;\n"
                 << "  vec2 uv;\n"
                 << "  flat uint tid;\n"
                 << "  vec4 color;\n"
                 << "} vertex;\n"
                 //
                 << "void main()\n"
                 << "{\n"
                 << "  color = vertex.color * texture("
                    << U_SAMPLER_ARRAY << "[vertex.tid], vertex.uv);\n"
                 << "}\n";

            f_src = frag.str();
        }

        return f_src;
    }

    static std::string unavailable;
    return unavailable;
}

} // namespace rdge
