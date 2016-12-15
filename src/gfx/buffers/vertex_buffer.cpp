#include <rdge/gfx/buffers/vertex_buffer.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

using namespace rdge::gfx;

VertexBuffer::VertexBuffer (float* data, rdge::int32 count, rdge::uint32 component_count)
    : m_componentCount(component_count)
{
    m_bufferId = opengl::CreateBuffer();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    opengl::SetBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_STATIC_DRAW);
    opengl::BindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer (void)
{
    glDeleteBuffers(1, &m_bufferId);
}

void
VertexBuffer::Bind (void) const
{
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_bufferId);
}

void
VertexBuffer::Unbind (void) const
{
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);
}
