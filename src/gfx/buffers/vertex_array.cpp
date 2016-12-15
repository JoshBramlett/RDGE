#include <rdge/gfx/buffers/vertex_array.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

using namespace rdge::gfx;

VertexArray::VertexArray (void)
{
    m_arrayId = opengl::CreateVertexArray();
}

VertexArray::~VertexArray (void)
{
    // TODO:  This code is a HUGE code smell.  The Buffer object has only the
    //        default destructor, so if he assumes deleting it will free the
    //        memory he's wrong.  Also, why give responsibility to the container?
    //        Especially since memory allocation is done outside any object
    //        instantiation.
    for (unsigned long i = 0; i < m_buffers.size(); i++)
    {
        delete m_buffers[i];
    }
}

void
VertexArray::AddBuffer (VertexBuffer* buffer, rdge::uint32 index)
{
    Bind();
    buffer->Bind();

    opengl::EnableVertexAttribute(index);
    opengl::SetVertexAttributePointer(index, buffer->ComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);

    buffer->Unbind();
    Unbind();
}

void
VertexArray::Bind (void) const
{
    opengl::BindVertexArray(m_arrayId);
}

void
VertexArray::Unbind (void) const
{
    opengl::UnbindVertexArrays();
}
