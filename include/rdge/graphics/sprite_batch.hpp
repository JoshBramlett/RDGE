//! \headerfile <rdge/graphics/sprite_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/03/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>
//#include <rdge/graphics/irenderable.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/texture.hpp>

// TODO remove when irenderable
#include <rdge/graphics/sprite.hpp>

#include <memory>
#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \class SpriteBatch
//! \details 2D renderer that pre-allocates a large buffer for ultra fast batch
//!          rendering.  When renderables are submitted their attributes are
//!          written directly to a mapped memory location.  The renderer requires
//!          all renderables are made up of 4 vertices.
class SpriteBatch
{
public:
    //! \struct vertex_attributes
    //! \brief The organization of vertex attributes in the buffer
    //! \details The shader used with the SpriteBatch renderer must adhere to this
    //!          vertex attribute definition.
    struct vertex_attributes
    {
        math::vec3 pos;   //!< Vertex coordinates
        math::vec2 uv;    //!< Texture (UV) coordinates
        uint32     tid;   //!< Texture Unit ID
        uint32     color; //!< Vertex color
    };

    //!@{
    //! \brief Required shader fields/values
    //! \details Any shader which is used by this renderer must adhere to the required
    //!          fields/values.  The layout locations defined in the vertex shader are
    //!          a direct mapping to the /ref vertex_attribute struct, and the uniforms
    //!          are used to set the projection matrix and texture data.
    static constexpr uint32 LAYOUT_LOC_POS   = 0; //!< Position attribute index
    static constexpr uint32 LAYOUT_LOC_UV    = 1; //!< UV coordinates attribute index
    static constexpr uint32 LAYOUT_LOC_TID   = 2; //!< Texture Unit ID attribute index
    static constexpr uint32 LAYOUT_LOC_COLOR = 3; //!< Color attribute index

    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix"; //!< Projection matrix uniform
    static constexpr const char* UNI_SAMPLER_ARR = "textures";    //!< Sampler2D array uniform
    //!@}

    //! \brief SpriteBatch default ctor
    //! \details Creates a rendering buffer for the provided sprite count.  If no
    //!          shader is provided a default shader will be used.  The projection
    //!          matrix will an orthographic projection based on the aspect ratio
    //!          of the OpenGL viewport.
    //! \param [in] num_sprites Max number of sprites that can be submitted
    //! \param [in] shader Initialized shader
    //! \warning Shader source must follow the required fields/values specification
    explicit SpriteBatch (uint32 num_sprites = 1000,
                          std::shared_ptr<Shader> shader = nullptr);

    //! \brief SpriteBatch dtor
    ~SpriteBatch (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    SpriteBatch (const SpriteBatch&) = delete;
    SpriteBatch& operator= (const SpriteBatch&) = delete;
    SpriteBatch (SpriteBatch&&) noexcept;
    SpriteBatch& operator= (SpriteBatch&&) noexcept;
    //!@}

    void RegisterTexture (Texture& texture);

    //! \brief Prepares for renderable submissions
    //! \details Should be called every frame prior to renderable submissions.
    //! \warning This is a required call before any renderables are submitted
    void PrepSubmit (void);

    //! \brief Submit a renderable for drawing
    //! \param [in] renderable Renderable to be drawn
    //void Submit (const IRenderable2D& renderable);
    void Submit (const Sprite& renderable);

    //! \brief Draw the contents of the buffer
    //! \details Should be called every frame after all renderable submissions.
    void Flush (void);





    // TODO
    //void SetColor (const rdge::color& color);

    // https://github.com/SFML/SFML/blob/master/include/SFML/Graphics/BlendMode.hpp
    // libgdx::SpriteBatch setBlendFunction
    //void DisableBlending ();
    //void EnableBlending ();
    //void SetBlendingFunction (...)

    void SetProjection (const math::mat4& projection_matrix);
    //void SetProjection (const rdge::Camera& camera);

    // Sparky computes the transform on the CPU, libgdx passes the matrix to the shader.
    // I like the latter, but keep in mind one problem with this is when you do the
    // transform outside of the sprite, I could not get ray casting to work because the
    // object didn't know it's final position.  To fix that I could provide a way to
    // have the sprite compute it's final position and not send the transform to the
    // renderer.  Alternatively (if works), I could do an inverse projection?
    //void SetTransformation (const math::mat4& xform);
    //void PushTransformation (const math::mat4& xform);
    //void PopTransformation (void);

private:
    uint32 m_vao = 0; //!< Vertex array handle
    uint32 m_vbo = 0; //!< Vertex buffer handle
    uint32 m_ibo = 0; //!< Index buffer handle

    vertex_attributes* m_cursor = nullptr;   //!< Cursor for current submission
    uint32             m_submissions = 0;    //!< Number of submissions between draw calls
    uint32             m_maxSubmissions = 0; //!< Max number of submissions per draw

    std::unique_ptr<uint[]> m_indexTable; //!< Data storage for the index buffer
    std::shared_ptr<Shader> m_shader;     //!< Shader program
    math::mat4              m_projection; //!< Projection matrix provided to shader

    std::vector<math::mat4> m_transformStack;      //!< Rendering transformation stack
    math::mat4*             m_transform = nullptr; //!< Current transformation


    std::vector<std::shared_ptr<Texture>> m_textures;

};

} // namespace rdge
