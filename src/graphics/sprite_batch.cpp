#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <algorithm> // std::generate
#include <sstream>

namespace {

using namespace rdge;

constexpr uint32 VERTEX_SIZE = sizeof(sprite_vertex);
constexpr uint32 SPRITE_SIZE = VERTEX_SIZE * 4;

} // anonymous namespace

namespace rdge {

SpriteBatch::SpriteBatch (uint16 num_sprites, std::shared_ptr<Shader> shader)
    : m_maxSubmissions(num_sprites)
    , m_shader(shader)
{
    SDL_assert(num_sprites != 0);

    if (!shader)
    {
        auto source = DefaultShaderSource();
        m_shader = std::make_unique<Shader>(source.first, source.second);
    }

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

    uint32 vbo_size  = static_cast<uint32>(m_maxSubmissions) * SPRITE_SIZE;
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VATTR_POS_INDEX);
    opengl::VertexAttribPointer(VATTR_POS_INDEX,
                                3,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, pos)));

    opengl::EnableVertexAttribute(VATTR_UV_INDEX);
    opengl::VertexAttribPointer(VATTR_UV_INDEX,
                                2,
                                GL_FLOAT,
                                false,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, uv)));

    opengl::EnableVertexAttribute(VATTR_TID_INDEX);
    opengl::VertexAttribIPointer(VATTR_TID_INDEX, // Note the "I" - required for integer types
                                 1,
                                 GL_UNSIGNED_INT,
                                 VERTEX_SIZE,
                                 reinterpret_cast<void*>(offsetof(sprite_vertex, tid)));

    opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);
    opengl::VertexAttribPointer(VATTR_COLOR_INDEX,
                                4,
                                GL_UNSIGNED_BYTE,
                                true,
                                VERTEX_SIZE,
                                reinterpret_cast<void*>(offsetof(sprite_vertex, color)));

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // IBO
    uint32 ibo_count = static_cast<uint32>(m_maxSubmissions) * 6;
    uint32 ibo_size  = ibo_count * sizeof(uint32);

    m_ibo = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_iboData = std::make_unique<uint32[]>(ibo_count);
    if (UNLIKELY(!m_iboData))
    {
        RDGE_THROW("Memory allocation failed");
    }

    for (uint32 i = 0, idx = 0, offset = 0;
         i < m_maxSubmissions;
         i++, idx = i * 6, offset = i * 4)
    {
        m_iboData[idx]     = offset;
        m_iboData[idx + 1] = offset + 1;
        m_iboData[idx + 2] = offset + 2;

        m_iboData[idx + 3] = offset + 2;
        m_iboData[idx + 4] = offset + 3;
        m_iboData[idx + 5] = offset;
    }

    opengl::SetBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_size, m_iboData.get(), GL_STATIC_DRAW);
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);

    opengl::UnbindVertexArrays();

    // A requirement we impose on the fragment shader is to define a sampler2D array
    // with the element count equal to the maximum texture units available.  A vector
    // is filled with values { 0 ... MAX-1 } and set to the uniform.
    std::vector<int32> texture_units(Shader::MaxFragmentShaderUnits());
    int32 n = 0;
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    // TODO Projection is hard-coded until I figure out how to implement it
    //      correctly.  Currently we're setting the NDC using the aspect ratio,
    //      but other engines have been setting it to the viewport.
    m_projection = math::mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);

    m_shader->Enable();
    m_shader->SetUniformValue(UNI_PROJ_MATRIX, m_projection);
    m_shader->SetUniformValue(UNI_SAMPLER_ARR, texture_units.size(), texture_units.data());
    m_shader->Disable();

    // Create and register a single white pixel texture that will be associated with
    // all sprites that do not have a texture (i.e. color only).  In the fragment
    // shader the color is multiplied by the texture color to provide the final color,
    // so this "dummy" texture will be a no-op.  This avoids having conditional logic
    // in our shader code to determine whether the sprite has an associated texture.
    // Non-uniform flow control has a major negative performance impact.
    // https://www.opengl.org/wiki/Sampler_(GLSL)#Non-uniform_flow_control
    uint32 pixel = 0xFFFFFFFF;
    auto dummy = SDL_CreateRGBSurfaceFrom(static_cast<void*>(&pixel),
                                          1, 1, 32, 4, 0, 0, 0, 0);
    RegisterTexture(std::make_shared<Texture>(Surface(dummy))); // Surface temp will free 'dummy'

    // An identity matrix is placed at the base of the transform stack and used
    // as a no-op transform if none are provided.
    PushTransformation(math::mat4::identity(), true);

    std::ostringstream ss;
    ss << "SpriteBatch[" << this << "] constructed:"
       << " sprite_count=" << m_maxSubmissions
       << " vbo_size=" << vbo_size
       << " ibo_size=" << ibo_size;
    DLOG(ss.str());
}

SpriteBatch::~SpriteBatch (void) noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

SpriteBatch::SpriteBatch (SpriteBatch&& rhs) noexcept
    : m_cursor(rhs.m_cursor)
    , m_submissions(rhs.m_submissions)
    , m_maxSubmissions(rhs.m_maxSubmissions)
    , m_iboData(std::move(rhs.m_iboData))
    , m_shader(std::move(rhs.m_shader))
    , m_projection(rhs.m_projection)
    , m_transformStack(std::move(rhs.m_transformStack))
    , m_transform(rhs.m_transform)
    , m_textures(std::move(rhs.m_textures))
{
    // Swap used so moved-from object dtor can cleanup
    std::swap(m_vao, rhs.m_vao);
    std::swap(m_vbo, rhs.m_vbo);
    std::swap(m_ibo, rhs.m_ibo);

    rhs.m_cursor = nullptr;
    rhs.m_submissions = 0;
    rhs.m_maxSubmissions = 0;
    rhs.m_transform = nullptr;
}

SpriteBatch&
SpriteBatch::operator= (SpriteBatch&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_cursor = rhs.m_cursor;
        m_submissions = rhs.m_submissions;
        m_maxSubmissions = rhs.m_maxSubmissions;
        m_iboData = std::move(rhs.m_iboData);
        m_shader = std::move(rhs.m_shader);
        m_projection = rhs.m_projection;
        m_transformStack = std::move(rhs.m_transformStack);
        m_transform = rhs.m_transform;
        m_textures = std::move(rhs.m_textures);

        std::swap(m_vao, rhs.m_vao);
        std::swap(m_vbo, rhs.m_vbo);
        std::swap(m_ibo, rhs.m_ibo);

        rhs.m_cursor = nullptr;
        rhs.m_submissions = 0;
        rhs.m_maxSubmissions = 0;
        rhs.m_transform = nullptr;
    }

    return *this;
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

    std::ostringstream ss;
    ss << "SpriteBatch[" << this << "] texture registered:"
       << " texture=" << texture.get()
       << " unit_id=" << texture->unit_id;
    DLOG(ss.str());
}

void
SpriteBatch::PrepSubmit (void)
{
    m_shader->Enable();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_cursor = reinterpret_cast<sprite_vertex*>(opengl::GetBufferPointer(GL_ARRAY_BUFFER,
                                                                             GL_WRITE_ONLY));

    m_submissions = 0;
}

void
SpriteBatch::Submit (const SpriteVertices& vertices)
{
    SDL_assert(m_submissions <= m_maxSubmissions);
    SDL_assert(m_vbo == static_cast<uint32>(opengl::GetIntegerValue(GL_ARRAY_BUFFER_BINDING)));

    for (const auto& vertex : vertices)
    {
        m_cursor->pos   = *m_transform * vertex.pos;
        m_cursor->uv    = vertex.uv;
        m_cursor->tid   = (vertex.tid == Texture::INVALID_UNIT_ID) ? 0 : vertex.tid;
        m_cursor->color = vertex.color;
        m_cursor++;
    }

    m_submissions++;
}

void
SpriteBatch::Flush (void)
{
    // Sanity check our VBO is bound ensures noone else is binding a different VBO
    // during our submission process.
    SDL_assert(m_vbo == static_cast<uint32>(opengl::GetIntegerValue(GL_ARRAY_BUFFER_BINDING)));
    SDL_assert(m_submissions != 0);

    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    for (auto& texture : m_textures)
    {
        texture->Activate();
    }

    opengl::BindVertexArray(m_vao);
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    opengl::DrawElements(GL_TRIANGLES, (m_submissions * 6), GL_UNSIGNED_INT, nullptr);

    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    opengl::UnbindVertexArrays();
}

void
SpriteBatch::PushTransformation (const rdge::math::mat4& matrix, bool override)
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

std::pair<std::string, std::string>
SpriteBatch::DefaultShaderSource (void) const
{
    std::ostringstream vert;
    vert << "#version 330 core\n"
         //
         << "layout (location = " << VATTR_POS_INDEX   << ") in vec4 position;\n"
         << "layout (location = " << VATTR_UV_INDEX    << ") in vec2 uv;\n"
         << "layout (location = " << VATTR_TID_INDEX   << ") in uint tid;\n"
         << "layout (location = " << VATTR_COLOR_INDEX << ") in vec4 color;\n"
         //
         << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
         //
         << "out vertex_attributes"
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
         << "  gl_Position  = " << UNI_PROJ_MATRIX << " * position;\n"
         << "}\n";

    std::ostringstream frag;
    frag << "#version 330 core\n"
         //
         << "layout (location = 0) out vec4 color;\n"
         //
         << "uniform sampler2D " << UNI_SAMPLER_ARR << "[" << Shader::MaxFragmentShaderUnits() << "];\n"
         //
         << "in vertex_attributes"
         << "{\n"
         << "  vec4 pos;\n"
         << "  vec2 uv;\n"
         << "  flat uint tid;\n"
         << "  vec4 color;\n"
         << "} vertex;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  color = vertex.color * texture(" << UNI_SAMPLER_ARR << "[vertex.tid], vertex.uv);\n"
         << "}\n";

    return std::make_pair(vert.str(), frag.str());
}

} // namespace rdge
