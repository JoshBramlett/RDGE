//! \headerfile <rdge/graphics/renderers/sprite_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/17/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/blend.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/mat4.hpp>

#include <memory>
#include <vector>
#include <array>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
struct sprite_data;
class OrthographicCamera;
//!@}

//! \class SpriteBatch
//! \details 2D renderer that writes vertex data to a pre-allocated buffer for ultra
//!          fast batch rendering.  The buffer created is based upon 4 vertices per
//!          sprite, where each vertex attributes are defined by \ref sprite_vertex.
//!          If a custom shader is used it must adhere to this definition.
class SpriteBatch
{
public:
    //!@{ Required shader vertex attributes
    static constexpr uint16 VA_POS_INDEX   = 0; //!< Position attribute index
    static constexpr uint16 VA_UV_INDEX    = 1; //!< UV coordinates attribute index
    static constexpr uint16 VA_TID_INDEX   = 2; //!< Sampler slot attribute index
    static constexpr uint16 VA_COLOR_INDEX = 3; //!< Color attribute index
    //!@}

    //!@{ Required shader uniforms
    static constexpr const char* U_PROJ_XF       = "u_proj_xf";  //!< Projection transform
    static constexpr const char* U_SAMPLER_ARRAY = "u_textures"; //!< Sampler2D array
    //!@}

    //! \brief SpriteBatch default ctor
    //! \details Creates a rendering buffer for the provided capacity.  A default
    //!          shader will be compiled and used.
    //! \param [in] capacity Max number of sprites that can be submitted
    //! \throws rdge::Exception Initialization failure
    explicit SpriteBatch (uint16 capacity = 1000);

    //! \brief SpriteBatch dtor
    ~SpriteBatch (void) noexcept;

    //!@{ Non-copyable, move enabled
    SpriteBatch (const SpriteBatch&) = delete;
    SpriteBatch& operator= (const SpriteBatch&) = delete;
    SpriteBatch (SpriteBatch&&) noexcept;
    SpriteBatch& operator= (SpriteBatch&&) noexcept;
    //!@}

    //! \brief Register a texture with the renderer.
    //! \details Registering a texture ties it to the shader by assigning it
    //!          it's unit id, therefore textures should not be shared accross
    //!          different renderers.  In order to activate the texture to the
    //!          OpenGL context, a reference to the texture is cached.  The
    //!          number of registrations allowed is defined by the OpenGL
    //!          implementation, which can be queried by calling \ref
    //!          Shader::MaxFragmentShaderUnits().
    //! \note As a best practice, all registrations should occur before the
    //!       first call to \ref PrepSubmit.
    //! \param [in] texture Shared pointer to the texture
    //! \throws rdge::Exception Registered textures already at maximum allowed
    void RegisterTexture (std::shared_ptr<Texture> texture);

    //! \brief Prepare the renderer to process submissions
    //! \note This is a required call before any submissions.
    //! \warning Each instances submission process must flush before another
    //!          instance can begin.
    void PrepSubmit (void);
    void Prime (const OrthographicCamera& camera, Shader& shader);

    //! \brief Submit an array of sprite vertices for drawing
    //! \param [in] vertices Array of vertex data to be inserted into the buffer
    //! \note \ref PrepSubmit must be called before submissions.
    void Submit (const SpriteVertices& vertices);
    void Submit (const sprite_data& sprite);

    //! \brief Draw the contents of the buffer
    //! \details Should be called every frame after all submissions.
    void Flush (void);
    void Flush (const std::vector<Texture>& textures);

    //! \brief Set the viewport that will be rendered
    //! \details Should be called every frame prior to drawing.
    //! \param [in] camera Orthographic camera
    void SetView (const OrthographicCamera& camera);

    //! \brief Push a transformation on the stack
    //! \details Applies the transformation to all submitted renderables until
    //!          popped off the stack.  Transformations will accumulate, meaning
    //!          the pushed transformation will be a product of the provided
    //!          matrix and the the transform on the top of the stack.
    //! \param [in] matrix Transformation matrix
    //! \param [in] override If true push raw parameter on stack
    void PushTransformation (const math::mat4& matrix, bool override = false);

    //! \brief Pop the top-most transformation off the stack
    void PopTransformation (void);

    //! \brief SpriteBatch capacity (aka maximum submission count)
    //! \returns Capacity
    uint16 Capacity (void) const noexcept;

    // TODO - Transform
    // We perform the transform, libgdx passes the calculation to the shader via
    // uniform.  I'll need to reevaluate the pros/cons once I have a valid use case
    // for grouping transformations.  Note the transform will impact ray casting, so
    // I'll need to find a way to get the inverse.

    // TODO - Color
    // Global color is used to tint all sprites, which could be sent to the shader as
    // a uniform to multiply.
    //void SetColor (const color& color);

public:
    Blend blend = Blend::LerpSourceAlpha; //!< Blend function (set every draw call)

private:
    uint32 m_vao = 0; //!< Vertex array handle
    uint32 m_vbo = 0; //!< Vertex buffer handle
    uint32 m_ibo = 0; //!< Index buffer handle

    sprite_vertex* m_cursor = nullptr; //!< VBO cursor
    size_t m_submissions = 0;          //!< Tracks submissions per draw call
    size_t m_capacity = 0;             //!< Max number of submissions per draw

    Shader m_shader; //!< Shader program

    std::vector<math::mat4> m_transformStack;      //!< Rendering transformation stack
    math::mat4*             m_transform = nullptr; //!< Points to the top element of the stack

    std::vector<std::shared_ptr<Texture>> m_textures; //!< List of registered textures
};

} // namespace rdge
