//! \headerfile <rdge/gfx/buffers/vertex_array.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/28/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/gfx/buffers/vertex_buffer.hpp>

#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

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
    void AddBuffer (VertexBuffer* buffer, rdge::uint32 index);

    //! \brief Bind vertex array to the OpenGL context
    void Bind (void) const;

    //! \brief Unbind vertex array to the OpenGL context
    void Unbind (void) const;

private:
    rdge::uint32 m_arrayId;
    std::vector<VertexBuffer*> m_buffers;
};

} // namespace gfx
} // namespace rdge
