//! \headerfile <rdge/graphics/buffers/index_buffer.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/28/2016

#pragma once

#include <rdge/types.hpp>

#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class IndexBuffer
//! \brief Create an OpenGL buffer for indices to vertex data
//! \details An index buffer is designed to reduce the size of the vertex buffer
//!          since triangles often share vertices.  The indices provided map
//!          directly to values of the vertex buffer, specifying which vertex
//!          will be used to render the triangles.
//! \note The underlying type is an unsigned short, which carries a maximum of
//!       65536 elements.
class IndexBuffer
{
public:
    //! \typedef IndexBufferData
    //! \brief Index buffer data storage container
    using IndexBufferData = std::unique_ptr<RDGE::UInt16[]>;

    //! \brief IndexBuffer ctor
    //! \details Copies the provided data to the GPU.  Ownership of the data
    //!          is retained by the caller.
    //! \param [in] data Pointer to the array of indices
    //! \param [in] count Number of elements in the array
    explicit IndexBuffer (RDGE::UInt16* data, RDGE::UInt16 count);

    //! \brief IndexBuffer ctor
    //! \details Copies the provided data to the GPU.  Ownership of the data
    //!          is aquired by the object.
    //! \param [in] data Unique pointer storing the data
    explicit IndexBuffer (IndexBufferData data);

    //! \brief IndexBuffer dtor
    ~IndexBuffer (void);

    //! \brief IndexBuffer Copy ctor
    //! \details Non-copyable
    IndexBuffer (const IndexBuffer&) = delete;

    //! \brief IndexBuffer Move ctor
    //! \details Transfers ownership of data
    IndexBuffer (IndexBuffer&& rhs) noexcept;

    //! \brief IndexBuffer Copy Assignment Operator
    //! \details Non-copyable
    IndexBuffer& operator= (const IndexBuffer&) = delete;

    //! \brief IndexBuffer Move Assignment Operator
    //! \details Transfers ownership of data
    IndexBuffer& operator= (IndexBuffer&& rhs) noexcept;

    //! \brief Get the number of indices that make up the data
    //! \returns index count
    RDGE::UInt16 Count (void) const noexcept
    {
        return m_count;
    }

    //! \brief Bind buffer to the OpenGL context
    void Bind (void) const;

    //! \brief Unbind buffer to the OpenGL context
    void Unbind (void) const;

private:
    RDGE::UInt32    m_bufferId;
    RDGE::UInt16    m_count;
    IndexBufferData m_data;
};

} // namespace Graphics
} // namespace RDGE
