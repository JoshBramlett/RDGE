#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>
#include <rdge/color.hpp>

#include <GL/glew.h>

namespace RDGE {
namespace Graphics {

namespace {

    constexpr RDGE::UInt16 MAX_SPRITES  = 60000;
    constexpr RDGE::UInt32 VERTEX_SIZE  = sizeof(vertex_data);
    constexpr RDGE::UInt32 SPRITE_SIZE  = VERTEX_SIZE * 4;
    constexpr RDGE::UInt32 BUFFER_SIZE  = SPRITE_SIZE * static_cast<RDGE::UInt32>(MAX_SPRITES);
    constexpr RDGE::UInt32 INDICES_SIZE = static_cast<RDGE::UInt32>(MAX_SPRITES) * 6;

    constexpr RDGE::UInt32 SHADER_VERTEX_INDEX = 0;
    constexpr RDGE::UInt32 SHADER_COLOR_INDEX = 1;

} // anonymous namespace

Renderer2D::Renderer2D (void)
{
    m_transformationStack.push_back(RDGE::Math::mat4::identity());
    m_currentTransformation = &m_transformationStack.back();

    m_vao = OpenGL::CreateVertexArray();
    m_vbo = OpenGL::CreateBuffer();

    OpenGL::BindVertexArray(m_vao);
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Allocate the full size (empty) buffer
    OpenGL::SetBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    OpenGL::EnableVertexAttribute(SHADER_VERTEX_INDEX);
    OpenGL::EnableVertexAttribute(SHADER_COLOR_INDEX);
    OpenGL::SetVertexAttributePointer(
                                      SHADER_VERTEX_INDEX,
                                      3,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      nullptr
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
                                      SHADER_COLOR_INDEX,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      true,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, color))
                                     );
    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);

    IndexBufferData ibo_data(new RDGE::UInt32[INDICES_SIZE]);
    RDGE::UInt32 offset = 0;
    for (RDGE::UInt32 i = 0; i < INDICES_SIZE; i += 6)
    {
        ibo_data[i] = offset;
        ibo_data[i + 1] = offset + 1;
        ibo_data[i + 2] = offset + 2;

        ibo_data[i + 3] = offset + 2;
        ibo_data[i + 4] = offset + 3;
        ibo_data[i + 5] = offset;

        offset += 4;
    }

    m_ibo = IndexBuffer(std::move(ibo_data), INDICES_SIZE);

    OpenGL::UnbindVertexArrays();
}

Renderer2D::~Renderer2D (void)
{
    glDeleteBuffers(1, &m_vbo);
}

void
Renderer2D::PrepSubmit (void)
{
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_buffer = reinterpret_cast<vertex_data*>(OpenGL::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void
Renderer2D::Submit (const Renderable2D* renderable)
{
    using namespace RDGE::Math;

    auto pos   = renderable->Position();
    auto size  = renderable->Size();
    auto color = renderable->Color().ToRgba();

    m_buffer->vertex = *m_currentTransformation * pos;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x, pos.y + size.y, pos.z);
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y + size.y, pos.z);
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y, pos.z);
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
