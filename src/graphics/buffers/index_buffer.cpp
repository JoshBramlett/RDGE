#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>

#include <GL/glew.h>

namespace RDGE {
namespace Graphics {

IndexBuffer::IndexBuffer (RDGE::UInt16* data, RDGE::UInt16 count)
    : m_count(count)
    , m_data(nullptr)
{
    m_bufferId = OpenGL::CreateBuffer();
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    OpenGL::SetBufferData(
                          GL_ELEMENT_ARRAY_BUFFER,
                          count * sizeof(RDGE::UInt16),
                          data,
                          GL_STATIC_DRAW
                         );
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer (IndexBufferData data, RDGE::UInt16 count)
    : m_count(count)
    , m_data(std::move(data))
{
    m_bufferId = OpenGL::CreateBuffer();
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    OpenGL::SetBufferData(
                          GL_ELEMENT_ARRAY_BUFFER,
                          count * sizeof(RDGE::UInt16),
                          m_data.get(),
                          GL_STATIC_DRAW
                         );
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer (void)
{
    glDeleteBuffers(1, &m_bufferId);
}

IndexBuffer::IndexBuffer (IndexBuffer&& rhs) noexcept
    : m_data(std::move(rhs.m_data))
{ }

IndexBuffer&
IndexBuffer::operator= (IndexBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_data = std::move(rhs.m_data);
    }

    return *this;
}

void
IndexBuffer::Bind (void) const
{
    OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}

void
IndexBuffer::Unbind (void) const
{
    OpenGL::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
}

} // namespace Graphics
} // namespace RDGE
