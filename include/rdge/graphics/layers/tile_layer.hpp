//! \headerfile <rdge/graphics/layers/tile_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/debug/widgets/graphics_widget.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Tileset;
class TileBatch;
class OrthographicCamera;
struct delta_time;
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

//! \struct tilemap_grid
//! \brief The global renderable range of the tilemap
//! \details The global grid is in screen space which it breaks down into
//!          individual cells, which tile layers may or may not map to a
//!          renderable tile.  The grid is to be converted to world
//!          coordinates for each layer.
struct tilemap_grid
{
    TileRenderOrder render_order;
    math::ivec2 pos;         //!< Grid starting location cell coordinates
    math::uivec2 size;       //!< Grid size (in cells)

    math::uivec2 cell_size;  //!< Cell size (in pixels)
    math::uivec2 chunk_size; //!< Chunk size (in cells)
};

//! \struct tile_cell
//! \brief Renderable cell data of a tile map
struct tile_cell
{
    // TODO This should be cleaned up.  The uv_data and uv_datap variables are a
    //      work-around to get it working correctly.  Originally each tile cell held
    //      it's uv value, but when animations were added the uvs of the cells were
    //      changed to a double pointer so the animation could be updated externally
    //      from the tile_cell.  Now tile_cells which do not reference an animation
    //      still store the uv, and use the uv_datap as a pointer to that local
    //      storage, with the uvs being a pointer to uv_datap.  Super ugly.
    //
    //      The major benefit of double pointer is that multiple tile_cells can now
    //      reference the same uv.  Since this is common in a tile map everything
    //      should be moved to this paradigm.
    //
    //      Problem to solve:
    //      - Cannot simply create a uv list from the Tileset
    //        - Some cells have rotation/flipping, which would increase the size
    //        - Not all cells are mapped, which would decrease the size
    //
    //      The only way to solve the above is to walk the list from the Tilemap first.
    //      Ideally this would not be done at runtime, so I'd have to update the
    //      cooker to include a field which designates the number of distinct mappings
    //      from the Tilemap.

    math::vec2 pos;       //!< Position in world coordinates
    tex_coords** uvs;     //!< Pointer to active UV coordinates
    tex_coords* uv_datap; //!< TODO Cleanup
    tex_coords uv_data;   //!< TODO Cleanup
};

//! \struct tile_chunk
//! \brief Chunk of cells in the global grid
struct tile_cell_chunk
{
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
    explicit TileLayer (const tilemap::Layer& def, float scale);
    ~TileLayer (void) noexcept;

    //!@{ Non-copyable, move enabled
    TileLayer (const TileLayer&) = delete;
    TileLayer& operator= (const TileLayer&) = delete;
    TileLayer (TileLayer&&) noexcept;
    TileLayer& operator= (TileLayer&&) noexcept;
    //!@}

    //! \brief Draw all tiles within the camera bounds
    void Draw (TileBatch& renderer, const OrthographicCamera& camera);

    //! \brief Update animated tiles
    void Update (const delta_time&);

private:
    friend class rdge::debug::GraphicsWidget;

    //! \struct chunk_grid
    //! \brief Quadrilateral subregion of the tilemap grid
    //! \details Chunks further break down the global grid into fixed size
    //!          containers, and therefore have their own coordinate system.
    struct chunk_grid
    {
        tile_cell_chunk* data = nullptr; //!< List of chunk data
        size_t count = 0;                //!< Chunk count in the global grid
        size_t rows = 0;                 //!< Chunk row count
        size_t cols = 0;                 //!< Chunk column count
    };

    tilemap_grid m_grid;
    tile_cell* m_cells = nullptr;
    chunk_grid m_chunks;

    math::vec2 m_offset;           //!< Start offset (in pixels)
    physics::aabb m_bounds;        //!< Layer boundary (in pixels)
    color m_color = color::WHITE;  //!< Render color (to store opacity)
    math::vec2 m_inv;              //!< Inverse pixel to chunk ratio

    //! \struct cell_animation
    //! \brief Animation for a single tile
    //! \details Tile animations loop and move in a singular forward direction.
    //!          The \ref tile_cell holds a pointer to the current_uv, which is
    //!          updated based upon the duration of the frames from the Update call.
    struct cell_animation
    {
        struct cell_frame
        {
            tex_coords uvs;     //!< UV coordinates for the frame
            uint32 duration;    //!< Duration of the frame (in milliseconds)
        };

        tex_coords* current_uv; //!< Pointer to the current frame in the animation
        size_t current_frame;   //!< Current frame index
        size_t elapsed;         //!< Elapsed time in the animation

        cell_frame* frames;     //!< Frame array
        size_t frame_count;     //!< Number of frames in the animation
    };

    //!@{ Animation members
    cell_animation* m_animations = nullptr;
    size_t m_animationCount = 0;
    cell_animation::cell_frame* m_frames = nullptr;
    size_t m_frameCount = 0;
    //!@}

public:
    std::string name; //!< Layer name
    Texture texture;  //!< Tileset texture

#ifdef RDGE_DEBUG
public:
    struct debug_overlay_data
    {
        bool hide_layer = false;
        bool draw_renderable_area = false;
        size_t chunks_drawn = 0;
    } debug_overlay;
#endif
};

//! \brief TileRenderOrder stream output operator
std::ostream& operator<< (std::ostream&, TileRenderOrder);

//!@{ TileRenderOrder string conversions
bool try_parse (const std::string&, TileRenderOrder&);
std::string to_string (TileRenderOrder);
//!@}

} // namespace rdge
