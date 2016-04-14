//! \headerfile <rdge/graphics/renderer2D.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/06/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/math/mat4.hpp>

#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

class Renderable2D;

//! \struct vertex_data
//! \brief Represents the organization of vertex data stored in the buffer
//! \details The data is for a single vertex, and is used to provide a
//!          logical mapping to the data allocated in the GPU
struct vertex_data
{
    RDGE::Math::vec3 vertex;
    RDGE::UInt32     color;
};

class Renderer2D
{
public:
    Renderer2D (void);

    ~Renderer2D (void);

    void PrepSubmit (void);

    void Submit (const Renderable2D* renderable);

    void EndSubmit (void);

    void Flush (void);

    void PushTransformation (RDGE::Math::mat4 matrix, bool override = false);

    void PopTransformation (void);

private:
    RDGE::UInt32 m_vao;
    RDGE::UInt32 m_vbo;
    vertex_data* m_buffer;

    IndexBuffer m_ibo;
    RDGE::Int32 m_indexCount;

    std::vector<RDGE::Math::mat4> m_transformationStack;
    RDGE::Math::mat4*             m_currentTransformation;
};

} // namespace Graphics
} // namespace RDGE
