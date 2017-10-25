//! \headerfile <rdge/graphics/renderers/tilemap_batch.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/18/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/physics/aabb.hpp>

#include <memory>
#include <vector>
#include <array>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class OrthographicCamera;
class Texture;
//!@}

//! \class TilemapBatch
//! \brief Renderer for a pre-defined tile map
class TilemapBatch
{
public:
    //!@{
    //! \brief Required shader fields/values
    static constexpr uint32 VATTR_POS_INDEX   = 0; //!< Position attribute index
    static constexpr uint32 VATTR_UV_INDEX    = 1; //!< UV coordinates attribute index
    static constexpr uint32 VATTR_COLOR_INDEX = 2; //!< Color attribute index

    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix"; //!< Projection matrix uniform
    //!@}

    //! \brief TilemapBatch default ctor
    TilemapBatch (void) = default;

    //! \brief TilemapBatch ctor
    //! \details Creates a copy of the \ref tilemap_data from the provided \ref
    //!          SpriteSheet.  Setup for all OpenGL performed.
    //! \param [in] sheet SpriteSheet owning the tile map and texture
    //! \param [in] scale Factor to scale the tiles drawing size
    //! \throws rdge::Exception Initialization failure
    explicit TilemapBatch (const SpriteSheet& sheet, float scale = 1.f);

    //! \brief TilemapBatch dtor
    ~TilemapBatch (void) noexcept;

    //!@{ Non-copyable, move enabled
    TilemapBatch (const TilemapBatch&) = delete;
    TilemapBatch& operator= (const TilemapBatch&) = delete;
    TilemapBatch (TilemapBatch&&) noexcept;
    TilemapBatch& operator= (TilemapBatch&&) noexcept;
    //!@}

    //! \brief Set the viewport that will be rendered
    //! \details Should be called every frame prior to drawing.
    //! \param [in] camera Orthographic camera
    void SetView (const OrthographicCamera& camera);

    //! \brief Draw the tilemap contents
    void Draw (void);

    // TODO
    // Limit rendering to camera boundaries
    // ImGui support to list drawn tiles per frame
    // Global color - tint all tiles
    // Blending on above layers

private:
    uint32 m_vao = 0; //!< Vertex array handle
    uint32 m_vbo = 0; //!< Vertex buffer handle
    uint32 m_ibo = 0; //!< Index buffer handle

    Shader        m_shader;    //!< Shader program
    physics::aabb m_bounds;    //!< View bounds to render
    float         m_far = 0.f; //!< Far clipping plane

    tilemap_data             m_tilemap; //!< Tilemap to render
    std::shared_ptr<Texture> m_texture; //!< Tilemap texture
};

} // namespace rdge
