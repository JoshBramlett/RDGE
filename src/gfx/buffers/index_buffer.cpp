#include <rdge/gfx/buffers/index_buffer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <GL/glew.h>

#include <algorithm>

using namespace rdge;
using namespace rdge::gfx;

IndexBuffer::IndexBuffer (void)
    : m_bufferId(0)
    , m_count(0)
    , m_data(nullptr)
{ }

IndexBuffer::IndexBuffer (rdge::uint32* data, rdge::uint32 count)
    : m_count(count)
    , m_data(nullptr)
{
    m_bufferId = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    opengl::SetBufferData(
                          GL_ELEMENT_ARRAY_BUFFER,
                          count * sizeof(rdge::uint32),
                          data,
                          GL_STATIC_DRAW
                         );
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
}

IndexBuffer::IndexBuffer (IndexBufferData&& data, rdge::uint32 count)
    : m_count(count)
    , m_data(std::move(data))
{
    m_bufferId = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    opengl::SetBufferData(
                          GL_ELEMENT_ARRAY_BUFFER,
                          count * sizeof(rdge::uint32),
                          m_data.get(),
                          GL_STATIC_DRAW
                         );
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
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
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}

void
IndexBuffer::Unbind (void) const
{
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
}
