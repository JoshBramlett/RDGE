#include <rdge/graphics/buffers/vertex_array.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>

#include <iostream>

namespace RDGE {
namespace Graphics {

VertexArray::VertexArray (void)
{
    m_arrayId = OpenGL::CreateVertexArray();
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
VertexArray::AddBuffer (VertexBuffer* buffer, GLuint index)
{
    Bind();
    buffer->Bind();

    OpenGL::EnableVertexAttribute(index);
    OpenGL::SetVertexAttributePointer(index, buffer->ComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);

    buffer->Unbind();
    Unbind();
}

void
VertexArray::Bind (void) const
{
    OpenGL::BindVertexArray(m_arrayId);
}

void
VertexArray::Unbind (void) const
{
    OpenGL::UnbindVertexArrays();
}

} // namespace Graphics
} // namespace RDGE
