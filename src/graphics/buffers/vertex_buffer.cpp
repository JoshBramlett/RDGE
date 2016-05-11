#include <rdge/graphics/buffers/vertex_buffer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace RDGE {
namespace Graphics {

VertexBuffer::VertexBuffer (GLfloat* data, GLsizei count, GLuint component_count)
    : m_componentCount(component_count)
{
    m_bufferId = OpenGL::CreateBuffer();
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    OpenGL::SetBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, GL_STATIC_DRAW);
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer (void)
{
    glDeleteBuffers(1, &m_bufferId);
}

void
VertexBuffer::Bind (void) const
{
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_bufferId);
}

void
VertexBuffer::Unbind (void) const
{
    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);
}

} // namespace Graphics
} // namespace RDGE
