//! \headerfile <rdge/graphics/layers/tile_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/aabb.hpp>

#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Texture;
class Tileset;
class TilemapBatch;
class OrthographicCamera;
namespace tilemap { class Layer; }
//!@}

//! \enum TileRenderOrder
//! \brief The order in which tiles are rendered
//! \details Useful when tiles require a certain z-indexing.  Only available with
//!          orthogonal maps.  In all cases, the map is drawn row-by-row
//! \see https://github.com/bjorn/tiled/issues/455
enum class TileRenderOrder
{
    INVALID = -1,
    RIGHT_DOWN,   //!< left-to-right and top-to-bottom
    RIGHT_UP,
    LEFT_DOWN,
    LEFT_UP
};

//! \struct tile_cell
//! \brief Renderable cell data of a tile map
struct tile_cell
{
    math::vec2 pos; //!< Position in world coordinates
    tex_coords uvs; //!< UV Coordinates
};

//! \struct tile_chunk
//! \brief Chunk of cells in the global grid
struct tile_cell_chunk
{
    math::svec2 location; //!< Chunk grid coordinates
    tile_cell* cells;     //!< List of cells
    size_t cell_count;    //!< Cell count per chunk
};

//! \class TileLayer
//! \brief Layer of a tilemap
//! \details Contains cell data used to render a layer of a tilemap with the
//!          \ref TilemapBatch render target.  Cell data should be considered
//!          immuatable and contain post-processed values, meaning scaling,
//!          rotation, etc. should be already calculated.
class TileLayer
{
public:
    explicit TileLayer (const tilemap::Layer& def, const Tileset& tileset, float scale);
    ~TileLayer (void) noexcept;

    //!@{ Non-copyable, move enabled
    TileLayer (const TileLayer&) = delete;
    TileLayer& operator= (const TileLayer&) = delete;
    TileLayer (TileLayer&&) noexcept = default;
    TileLayer& operator= (TileLayer&&) noexcept = default;
    //!@}

    //! \brief Draw all tiles within the camera bounds
    void Draw (TilemapBatch& renderer, const OrthographicCamera& camera);

private:

    //! \struct cell_grid
    //! \brief Quadrilateral grid of cell data
    //! \details Contains all data to be rendered by the layer.  Cell data will
    //!          be empty for any locations that omit tile data.
    struct cell_grid
    {
        tile_cell* data = nullptr; //!< List of cell data
        size_t count = 0;          //!< Cell count in the global grid
        size_t pitch = 0;          //!< Cells per row in the global grid

        math::vec2 size; //!< Cell size (in pixels)
    };

    //! \struct chunk_grid
    //! \brief Quadrilateral grid of chunk data
    //! \details Chunks further break down the global grid into fixed size
    //!          containers, and therefore have their own coordinate system.
    struct chunk_grid
    {
        tile_cell_chunk* data = nullptr; //!< List of chunk data
        size_t count = 0;                //!< Chunk count in the global grid
        size_t pitch = 0;                //!< Chunks per row in the global grid

        math::svec2 size;  //!< Chunk size (in cells)
    };

    cell_grid m_cells;
    chunk_grid m_chunks;

    math::vec2 m_offset;           //!< Start offset (in pixels)
    physics::aabb m_bounds;        //!< Layer boundary (in pixels)
    color m_color = color::WHITE;  //!< Render color (to store opacity)

    std::shared_ptr<Texture> texture; //!< Tileset texture

    //size_t m_rows = 0;   //!< Total row count (in cells)
    //size_t m_cols = 0;   //!< Total column count (in cells)
    //size_t m_startX = 0; //!< Top left x-coordinate accross all chunks
    //size_t m_startY = 0; //!< Top left y-coordinate accross all chunks

    //grid_chunk* m_chunks = nullptr; //!< List of chunks
    //size_t m_chunkCount = 0;        //!< Chunk count
    //size_t m_chunkRows = 0;         //!< Chunk row count (in cells)
    //size_t m_chunkCols = 0;         //!< Chunk column count (in cells)
    //tile_cell* m_cells = nullptr;

    //physics::aabb m_bounds;        //!< Layer boundary (in pixels)
    //math::vec2 m_cellSize;         //!< Cell size (in pixels)
    //math::vec2 m_offset;           //!< Start offset (in pixels)
    //color m_color = color::WHITE;  //!< Render color (to store opacity)

    //std::shared_ptr<Texture> texture; //!< Tileset texture
};

//! \brief TileRenderOrder stream output operator
std::ostream& operator<< (std::ostream&, TileRenderOrder);

//!@{ TileRenderOrder string conversions
bool try_parse (const std::string&, TileRenderOrder&);
std::string to_string (TileRenderOrder);
//!@}

} // namespace rdge
