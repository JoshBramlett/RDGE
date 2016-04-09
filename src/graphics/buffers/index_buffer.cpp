#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>

#include <GL/glew.h>

#include <algorithm>

namespace RDGE {
namespace Graphics {

IndexBuffer::IndexBuffer (void)
    : m_bufferId(0)
    , m_count(0)
    , m_data(nullptr)
{ }

IndexBuffer::IndexBuffer (RDGE::UInt16* data, RDGE::UInt32 count)
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
    OpenGL::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
}

IndexBuffer::IndexBuffer (IndexBufferData data, RDGE::UInt32 count)
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
    OpenGL::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
}

IndexBuffer::~IndexBuffer (void)
{
    glDeleteBuffers(1, &m_bufferId);
}

IndexBuffer::IndexBuffer (IndexBuffer&& rhs) noexcept
    : m_count(rhs.m_count)
    , m_data(std::move(rhs.m_data))
{
    // The destructor deletes the OpenGL buffer, therefore we swap the buffer
    // ids so the moved-from object will destroy the buffer it's replacing
    std::swap(m_bufferId, rhs.m_bufferId);
}

IndexBuffer&
IndexBuffer::operator= (IndexBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_bufferId, rhs.m_bufferId);
        m_count = rhs.m_count;
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
