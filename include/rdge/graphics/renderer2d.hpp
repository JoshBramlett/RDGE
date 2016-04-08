//! \headerfile <rdge/graphics/renderer2D.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/06/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

class Renderer2D
{
public:
    Renderer2D (void);

    ~Renderer2D (void);

    void Begin (void);

    void Submit (const Renderable2D& renderable);

    void End (void);

    void Flush (void);

private:
    RDGE::UInt32 m_vao;
    RDGE::UInt32 m_vbo;
    vertex_data*  m_buffer;

    IndexBuffer m_ibo;
    RDGE::Int32 m_indexCount;
};

} // namespace Graphics
} // namespace RDGE
