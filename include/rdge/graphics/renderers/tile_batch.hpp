//! \headerfile <rdge/graphics/renderers/tile_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/18/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/shaders/shader_program.hpp>
#include <rdge/graphics/blend.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/mat4.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
struct tile_cell_chunk;
struct color;
struct tile_vertex;
class OrthographicCamera;
class Texture;
//!@}

//! \class TileBatch
//! \brief Renderer for a pre-defined tile map
class TileBatch
{
public:
    //!@{ Required shader vertex attributes
    static constexpr uint16 VA_POS_INDEX   = 0; //!< Position attribute index
    static constexpr uint16 VA_UV_INDEX    = 1; //!< UV coordinates attribute index
    static constexpr uint16 VA_COLOR_INDEX = 2; //!< Color attribute index
    //!@}

    //!@{ Required shader uniforms
    static constexpr const char* U_PROJ_XF = "u_proj_xf"; //!< Projection transform
    static constexpr const char* U_SAMPLER = "u_texture"; //!< Sampler2D
    //!@}

    static constexpr int32 TEXTURE_UNIT_ID = 0; //!< Slot used by the shader

    //! \brief TileBatch default ctor
    TileBatch (void) = default;

    //! \brief TileBatch ctor
    //! \details Creates a copy of the \ref tilemap_data from the provided \ref
    //!          SpriteSheet.  Setup for all OpenGL performed.
    //! \param [in] capacity Max number of tiles rendered per draw call
    //! \param [in] tile_size Static fixed size for a tile cell (in pixels)
    //! \throws rdge::Exception Initialization failure
    explicit TileBatch (uint16 capacity, const math::vec2& tile_size);

    //! \brief TileBatch dtor
    ~TileBatch (void) noexcept;

    //!@{ Non-copyable, move enabled
    TileBatch (const TileBatch&) = delete;
    TileBatch& operator= (const TileBatch&) = delete;
    TileBatch (TileBatch&&) noexcept;
    TileBatch& operator= (TileBatch&&) noexcept;
    //!@}

    //! \brief Set the viewport that will be rendered
    //! \details The combined projection/view matrix is cached and provided to
    //!          each shader prior to submission.
    //! \note This should be called once at the beginning of each frame.
    //! \param [in] camera Updated camera
    void SetView (const OrthographicCamera& camera);

    //! \brief Prepare the renderer to receive tiles to draw
    //! \note Required call prior to drawing.
    void Prime (void);

    //! \brief Submit a tile chunk to be drawn
    //! \param [in] chunk Chunk of tiles to be added to the buffer
    //! \param [in] c Tinting color
    void Draw (const tile_cell_chunk& chunk, color c);

    //! \brief Flush the contents of the buffer
    //! \param [in] texture Texture to activate
    void Flush (const Texture& texture);

    // TODO
    // ImGui support to list drawn tiles per frame
    // Global color - tint all tiles
    // Blending on above layers

public:
    Blend blend = Blend::LerpSourceAlpha; //!< Blend function (set every draw call)

private:
    uint32 m_vao = 0; //!< Vertex array handle
    uint32 m_vbo = 0; //!< Vertex buffer handle
    uint32 m_ibo = 0; //!< Index buffer handle

    tile_vertex* m_cursor = nullptr; //!< VBO cursor
    size_t m_submissions = 0;        //!< Tracks submissions per draw call
    size_t m_capacity = 0;           //!< Max number of submissions per draw

    math::mat4 m_combined;  //!< Projection/View matrix provided to the shader
    float m_far = 0.f;      //!< Far clipping plane
    ShaderProgram m_shader; //!< Shader program
    math::vec2 m_tileSize;  //!< Tile size
};

} // namespace rdge
