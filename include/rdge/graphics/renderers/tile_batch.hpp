//! \headerfile <rdge/graphics/renderers/tile_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/18/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/blend.hpp>

#include <memory>

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
    //!@{
    //! \brief Required shader fields/values
    static constexpr uint32 VATTR_POS_INDEX   = 0; //!< Position attribute index
    static constexpr uint32 VATTR_UV_INDEX    = 1; //!< UV coordinates attribute index
    static constexpr uint32 VATTR_COLOR_INDEX = 2; //!< Color attribute index

    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix"; //!< Projection matrix uniform
    //!@}

    //! \brief TileBatch default ctor
    TileBatch (void) = default;

    //! \brief TileBatch ctor
    //! \details Creates a copy of the \ref tilemap_data from the provided \ref
    //!          SpriteSheet.  Setup for all OpenGL performed.
    //! \param [in] capacity Max number of tiles rendered per draw call
    //! \param [in] texture Tileset texture
    //! \throws rdge::Exception Initialization failure
    explicit TileBatch (uint16 capacity, const math::vec2& tile_size, std::shared_ptr<Texture> texture);

    //! \brief TileBatch dtor
    ~TileBatch (void) noexcept;

    //!@{ Non-copyable, move enabled
    TileBatch (const TileBatch&) = delete;
    TileBatch& operator= (const TileBatch&) = delete;
    TileBatch (TileBatch&&) noexcept;
    TileBatch& operator= (TileBatch&&) noexcept;
    //!@}

    //! \brief Set the viewport that will be rendered
    //! \details Should be called every frame prior to drawing.
    //! \param [in] camera Orthographic camera
    void SetView (const OrthographicCamera& camera);

    void Prime (void);

    //! \brief Draw the tilemap contents
    void Draw (const tile_cell_chunk& chunk, color c);

    void Flush (void);

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

    Shader     m_shader;    //!< Shader program
    math::vec2 m_tileSize;  //!< Tile size
    float      m_far = 0.f; //!< Far clipping plane

    tile_vertex* m_cursor;          //!< VBO cursor
    size_t       m_submissions = 0; //!< Tracks submissions per draw call
    size_t       m_capacity = 0;    //!< Max number of submissions per draw

    std::shared_ptr<Texture> m_texture; //!< Tilemap texture
};

} // namespace rdge
