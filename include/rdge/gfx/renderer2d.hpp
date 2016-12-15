//! \headerfile <rdge/gfx/renderer2D.hpp>
//! \author Josh Bramlett
//! \version 0.0.7
//! \date 05/21/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/gfx/buffers/index_buffer.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>

#include <memory>
#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

class Renderable2D;

//! \struct vertex_data
//! \brief Represents the organization of vertex data stored in the buffer
//! \details The data is for a single vertex, and is used to provide a
//!          logical mapping to the data allocated in the GPU
struct vertex_data
{
    //! \var vertex Vertex coordinates
    rdge::math::vec3 vertex;
    //! \var uv Texture coordinates
    rdge::math::vec2 uv;
    //! \var tid Texture unit ID
    rdge::int32      tid;
    //! \var color Vertex color
    rdge::uint32     color;
};

//! \class Renderer2D
//! \details 2D renderer that pre-allocates a large buffer for ultra fast batch
//!          rendering.  When renderables are submitted they are written directly
//!          to a mapped memory location provided by OpenGL.  The renderer
//!          assumes all renderables are made up of 4 vertices.
class Renderer2D
{
public:
    //! \var MAX_SUPPORTED_SPRITE_COUNT
    //! \brief Maximum number of sprites that can be submitted to the renderer
    static constexpr rdge::uint16 MAX_SUPPORTED_SPRITE_COUNT = 60000;

    //! \brief Renderer2D ctor
    //! \details Creates a vertex array with a vertex buffer whose size is
    //!          defined by the provided max sprite count.
    //! \param [in] max_sprite_count Max number of sprites that can be submitted
    //! \throws RDGE::Exception Invalid sprite count
    explicit Renderer2D (rdge::uint16 max_sprite_count = 5000);

    //! \brief Renderer2D dtor
    //! \details Cleans up vertex array and buffers
    ~Renderer2D (void);

    //! \brief Renderer2D Copy ctor
    //! \details Non-copyable
    Renderer2D (const Renderer2D&) = delete;

    //! \brief Renderer2D Move ctor
    //! \details Transfers ownership
    Renderer2D (Renderer2D&&) noexcept;

    //! \brief Renderer2D Copy Assignment Operator
    //! \details Non-copyable
    Renderer2D& operator= (const Renderer2D&) = delete;

    //! \brief Renderer2D Move Assignment Operator
    //! \details Transfers ownership
    Renderer2D& operator= (Renderer2D&&) noexcept;

    //! \brief Prepares the renderer for renderable submissions
    //! \details This is a required call before any renderables are submitted.
    //!          It will bind the buffer and get the mapped pointer location.
    void PrepSubmit (void);

    //! \brief Submit a renderable to be drawn
    //! \details Renderables will be added to the buffer.
    //! \param [in] renderable Pointer to the renderable to be drawn
    //! \throws RDGE::Exception If submissions exceeds max allowed (only
    //!         enabled if RDGE_DEBUG flag is set)
    void Submit (const Renderable2D* renderable);

    //! \brief Alerts the renderer the submit phase has ended
    //! \details This is a required call after all renderables have been
    //!          submitted.  The mapped buffer will be released.
    void EndSubmit (void);

    //! \brief Draw the contents of the buffer
    //! \details Flush should be called every frame after the submission
    //!          phase has completed.
    void Flush (void);

    //! \brief Push a transformation matrix on the stack
    //! \details Apply the transformation to all submitted renderables until
    //!          popped off the stack.  Transformations are accumulated, so
    //!          the stored transform is a multiplication of the provided
    //!          matrix and the matrix on top of the stack.
    //! \param [in] matrix Transformation matrix
    //! \param [in] override When true, transformation will not accumulate
    void PushTransformation (const rdge::math::mat4& matrix, bool override = false);

    //! \brief Pop the top-most transformation off the stack
    void PopTransformation (void);

    //! \brief Register a texture to the renderer.
    //! \details All textures must be registered with the renderer before
    //!          any renderables are submitted.  OpenGL limits the amount
    //!          of textures within a shader, so it's recommended renderables
    //!          share textures to limit the amount of active textures to
    //!          within the supported amount.  If the provided texture does
    //!          not have a valid sampler unit id, it will be assigned one.
    //! \param [in] texture Shared pointer to the texture
    //! \throws RDGE::Exception Attempting to register a new texture after the
    //!                         maximum amount has already been reached
    void RegisterTexture (std::shared_ptr<rdge::Texture>& texture);

private:
    rdge::uint32 m_vao;
    rdge::uint32 m_vbo;
    IndexBuffer  m_ibo;
    rdge::int32  m_indexCount;
    vertex_data* m_buffer;

    std::vector<std::shared_ptr<Texture>> m_textures;

    std::vector<rdge::math::mat4> m_transformationStack;
    rdge::math::mat4*             m_currentTransformation;

    rdge::uint16 m_submissionCount;
    rdge::uint16 m_maxSubmissions;
};

} // namespace gfx
} // namespace rdge
