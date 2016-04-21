#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/color.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <GL/glew.h>

#include <algorithm>

namespace RDGE {
namespace Graphics {

namespace {

    constexpr RDGE::UInt32 VERTEX_SIZE = sizeof(vertex_data);
    constexpr RDGE::UInt32 SPRITE_SIZE = VERTEX_SIZE * 4;

    // Vertex attribute indices (maps to vertex_data struct)
    constexpr RDGE::UInt32 VATTR_VERTEX_INDEX = 0;
    constexpr RDGE::UInt32 VATTR_UV_INDEX     = 1;
    constexpr RDGE::UInt32 VATTR_TID_INDEX    = 2;
    constexpr RDGE::UInt32 VATTR_COLOR_INDEX  = 3;

} // anonymous namespace

Renderer2D::Renderer2D (RDGE::UInt16 max_sprite_count)
    : m_buffer(nullptr)
    , m_submissionCount(0)
    , m_maxSubmissions(max_sprite_count)
{
    if (max_sprite_count > MAX_SUPPORTED_SPRITE_COUNT)
    {
        RDGE_THROW(
                   "Max sprite count cannot exceed limit of " +
                   std::to_string(MAX_SUPPORTED_SPRITE_COUNT)
                  );
    }

    // define sizes used for buffer allocation
    RDGE::UInt32 buffer_size  = static_cast<RDGE::UInt32>(max_sprite_count) * SPRITE_SIZE;
    RDGE::UInt32 indices_size = static_cast<RDGE::UInt32>(max_sprite_count) * 6;

    // push an identity matrix on the transformation stack so all submissions
    // wihtout a transformation will essentially be a no-op
    m_transformationStack.push_back(RDGE::Math::mat4::identity());
    m_currentTransformation = &m_transformationStack.back();

    m_vao = OpenGL::CreateVertexArray();
    m_vbo = OpenGL::CreateBuffer();

    OpenGL::BindVertexArray(m_vao);
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Allocate the full size (empty) buffer
    OpenGL::SetBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

    OpenGL::EnableVertexAttribute(VATTR_VERTEX_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_UV_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_TID_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_COLOR_INDEX);

    OpenGL::SetVertexAttributePointer(
                                      VATTR_VERTEX_INDEX,
                                      3,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, vertex))
                                     );

    OpenGL::SetVertexAttributePointer(
                                      VATTR_UV_INDEX,
                                      2,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, uv))
                                     );

    OpenGL::SetVertexAttributePointer(
                                      VATTR_TID_INDEX,
                                      1,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, tid))
                                     );

    // TODO:  Need to figure out what's going on with color.  Supposedly when using
    //        GL_UNSIGNED_BYTE it'll keep the byte order regardless of endian-ness.
    //        The byte order should be GL_RGBA but LUL is creating it as ABGR.  Why
    //        does this work, and test to see what happens when using uint
    //
    //  From StackOverflow:
    //        "GL_RGBA with GL_UNSIGNED_BYTE will store the bytes in RGBA order
    //        regardless whether the computer is little-endian or big-endian."
    //  http://stackoverflow.com/questions/7786187/opengl-texture-upload-unsigned-byte-vs-unsigned-int-8-8-8-8

    OpenGL::SetVertexAttributePointer(
                                      VATTR_COLOR_INDEX,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      true,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, color))
                                     );

    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);

    IndexBufferData ibo_data(new RDGE::UInt32[indices_size]);
    RDGE::UInt32 offset = 0;
    for (RDGE::UInt32 i = 0; i < indices_size; i += 6)
    {
        ibo_data[i] = offset;
        ibo_data[i + 1] = offset + 1;
        ibo_data[i + 2] = offset + 2;

        ibo_data[i + 3] = offset + 2;
        ibo_data[i + 4] = offset + 3;
        ibo_data[i + 5] = offset;

        offset += 4;
    }

    m_ibo = IndexBuffer(std::move(ibo_data), indices_size);

    OpenGL::UnbindVertexArrays();
}

Renderer2D::~Renderer2D (void)
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void
Renderer2D::RegisterTexture (std::shared_ptr<GLTexture>& texture)
{
    // no texture or already added
    if (!texture || texture->UnitID() >= 0)
    {
        return;
    }

    auto size = static_cast<RDGE::Int32>(m_textures.size());
    if (size >= (Shader::MaxFragmentShaderUnits() - 1))
    {
        RDGE_THROW(
                   "Unable to register texture.  Max limit of " +
                   std::to_string(Shader::MaxFragmentShaderUnits()) +
                   " already reached."
                  );
    }

    texture->SetUnitID(size);
    m_textures.emplace_back(texture->GetSharedPtr());
}

void
Renderer2D::PrepSubmit (void)
{
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_buffer = reinterpret_cast<vertex_data*>(OpenGL::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    m_submissionCount = 0;
}

void
Renderer2D::Submit (const Renderable2D* renderable)
{
#ifdef RDGE_DEBUG
    if (m_submissionCount++ > m_maxSubmissions)
    {
        RDGE_THROW(
                   "Renderable2D submissions exceeded maximum of " +
                   std::to_string(m_maxSubmissions)
                  );
    }
#endif

    using namespace RDGE::Math;

    auto pos   = renderable->Position();
    auto size  = renderable->Size();
    auto uv    = renderable->UV();
    auto tid   = static_cast<float>(renderable->TextureUnitID());
    RDGE::UInt32 color = 0;

    if (tid < 0.0f)
    {
        color = renderable->Color().ToRgba();
    }

    m_buffer->vertex = *m_currentTransformation * pos;
    m_buffer->uv = uv[0];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x, pos.y + size.y, pos.z);
    m_buffer->uv = uv[1];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y + size.y, pos.z);
    m_buffer->uv = uv[2];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y, pos.z);
    m_buffer->uv = uv[3];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_indexCount += 6;
}

void
Renderer2D::EndSubmit (void)
{
    OpenGL::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);
}

void
Renderer2D::Flush (void)
{
    for (auto texture : m_textures)
    {
        texture->Activate();
    }

    OpenGL::BindVertexArray(m_vao);
    m_ibo.Bind();

    OpenGL::DrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

    m_ibo.Unbind();
    OpenGL::UnbindVertexArrays();

    m_indexCount = 0;
}

void
Renderer2D::PushTransformation (RDGE::Math::mat4 matrix, bool override)
{
    if (override)
    {
        m_transformationStack.push_back(matrix);
    }
    else
    {
        m_transformationStack.push_back(m_transformationStack.back() * matrix);
    }

    m_currentTransformation = &m_transformationStack.back();
}

void
Renderer2D::PopTransformation (void)
{
    if (m_transformationStack.size() > 1)
    {
        m_transformationStack.pop_back();
    }

    m_currentTransformation = &m_transformationStack.back();
}

} // namespace Graphics
} // namespace RDGE
