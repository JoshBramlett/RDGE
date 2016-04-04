#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>

namespace RDGE {
namespace Graphics {

IndexBuffer::IndexBuffer (GLushort* data, GLsizei count)
    : m_count(count)
{
    m_bufferId = OpenGL::CreateBuffer();
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    OpenGL::SetBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLushort), data, GL_STATIC_DRAW);
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    //glGenBuffers(1, &m_bufferId);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLushort), data, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
IndexBuffer::Bind (void) const
{
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}

void
IndexBuffer::Unbind (void) const
{
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace Graphics
} // namespace RDGE
