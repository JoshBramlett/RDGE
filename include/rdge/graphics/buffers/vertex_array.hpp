//! \headerfile <rdge/graphics/buffers/vertex_array.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/28/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/buffers/vertex_buffer.hpp>

#include <GL/glew.h>

#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class VertexArray
//! \brief
//! \details
class VertexArray
{
public:
    //! \brief VertexArray ctor
    VertexArray (void);

    //! \brief VertexArray dtor
    ~VertexArray (void);

    //! \brief Add buffer to our vertex array
    //! \param [in] buffer Pointer to vertex buffer object
    //! \param [in] index Shader layout location index
    void AddBuffer (VertexBuffer* buffer, GLuint index);

    //! \brief Bind vertex array to the OpenGL context
    void Bind (void) const;

    //! \brief Unbind vertex array to the OpenGL context
    void Unbind (void) const;

private:
    GLuint m_arrayId;
    std::vector<VertexBuffer*> m_buffers;
};

} // namespace Graphics
} // namespace RDGE
