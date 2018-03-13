//! \headerfile <rdge/graphics/renderers/sprite_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/17/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/blend.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/shaders/sprite_batch_shader.hpp>
#include <rdge/math/mat4.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
struct sprite_data;
struct sprite_vertex;
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

    //!@{ Basic SpriteBatch properties
    uint16 Capacity (void) const noexcept;
    //!@}

    //! \brief Set the viewport that will be rendered
    //! \details The combined projection/view matrix is cached and provided to
    //!          each shader prior to submission.  This should be called once
    //!          at the beginning of each frame.
    //! \param [in] camera Updated camera
    void SetView (const OrthographicCamera& camera);

    //!@{
    //! \brief Prepare the renderer to receive sprites to draw
    //! \details If no shader is provided the default shader will be used.
    //! \note Required call prior to drawing.
    void Prime (void);
    void Prime (SpriteBatchShader&);
    //!@{

    //! \brief Submit a sprite to be drawn
    //! \param [in] sprite Sprite data to be added to the buffer
    void Draw (const sprite_data& sprite);

    //! \brief Draw the contents of the buffer
    //! \param [in] textures Array of textures to activate
    void Flush (const std::vector<Texture>& textures);

    //! \brief Push a transformation on the stack
    //! \details Applies the transformation to all submitted renderables until
    //!          popped off the stack.  Transformations will accumulate, meaning
    //!          the pushed transformation will be a product of the provided
    //!          matrix and the the transform on the top of the stack.
    //! \param [in] matrix Transformation matrix
    //! \param [in] override Do not accumulate with the current transformation
    void PushTransformation (const math::mat4& matrix, bool override = false);

    //! \brief Pop the top-most transformation off the stack
    void PopTransformation (void);

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

    math::mat4 m_combined;      //!< Projection/View matrix provided to the shader
    SpriteBatchShader m_shader; //!< Shader program

    std::vector<math::mat4> m_transformStack;      //!< Rendering transformation stack
    math::mat4*             m_transform = nullptr; //!< Points to the top element of the stack
};

} // namespace rdge
