//! \headerfile <rdge/graphics/buffers/index_buffer.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/28/2016

#pragma once

#include <rdge/types.hpp>

#include <GL/glew.h>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class IndexBuffer
//! \brief Create an OpenGL buffer for indices to vertex data
//! \details An index buffer is designed to reduce the size of the vertex buffer
//!          since triangles often share vertices.  The indices provided map
//!          directly to values of the vertex buffer, specifying which vertex
//!          will be used to render the triangles.
class IndexBuffer
{
public:
    //! \brief IndexBuffer ctor
    //! \details Copies the provided data to the GPU.
    //! \param [in] data Array of indices (ushort has a max of 65536)
    //! \param [in] count Number of elements in the array
    explicit IndexBuffer (GLushort* data, GLsizei count);

    //! \brief Get the number of indices that make up the data
    //! \returns index count
    GLushort Count (void) const noexcept
    {
        return m_count;
    }

    //! \brief Bind buffer to the OpenGL context
    void Bind (void) const;

    //! \brief Unbind buffer to the OpenGL context
    void Unbind (void) const;

private:
    GLuint   m_bufferId;
    GLushort m_count;
};

} // namespace Graphics
} // namespace RDGE
