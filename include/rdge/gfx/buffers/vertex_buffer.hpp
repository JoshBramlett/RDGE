//! \headerfile <rdge/gfx/buffers/vertex_buffer.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/28/2016

#pragma once

#include <rdge/core.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

//! \class VertexBuffer
//! \brief Create an OpenGL buffer for the provided data
//! \details Stores only the pointer to the data, so no memory allocation is
//!          performed.  Underlying data should not change and is specified to
//!          OpenGL as such.
class VertexBuffer
{
public:
    //! \brief VertexBuffer ctor
    //! \details Copies the provided data to the GPU.
    //! \param [in] data Array of data to be bound to OpenGL
    //! \param [in] count Number of elements in the array
    //! \param [in] component_count Number of components that make up the data
    //! \note Component count is the logical separation of the data.  For example,
    //!       is the data made up of a vec4 (count = 1) or 2 vec2s (count = 2).
    explicit VertexBuffer (float* data, rdge::int32 count, rdge::uint32 component_count = 1);

    //! \brief VertexBuffer dtor
    ~VertexBuffer (void);

    //! \brief Get the number of components that make up the data
    //! \returns Component count
    rdge::uint32 ComponentCount (void) const noexcept
    {
        return m_componentCount;
    }

    //! \brief Bind buffer to the OpenGL context
    void Bind (void) const;

    //! \brief Unbind buffer to the OpenGL context
    void Unbind (void) const;

private:
    rdge::uint32 m_bufferId;
    rdge::uint32 m_componentCount;
};

} // namespace gfx
} // namespace rdge
