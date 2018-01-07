//! \headerfile <rdge/graphics/layers/tile_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <memory>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Texture;
class Tileset;
namespace tilemap { class Layer; }
//!@}

//! \enum RenderOrder
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

    //! \brief Draw all cached sprites
    void Draw (void);

public:
    //! \struct tile_cell
    //! \brief Renderable cell of a tile map
    struct tile_cell
    {
        math::vec2 pos;    //!< Position in world coordinates
        tex_coords coords; //!< UV Coordinates
    };

    tile_cell* cells = nullptr;
    size_t cell_count = 0;
    size_t cell_pitch = 0;

    math::vec2 cell_size;
    math::vec2 offset;
    float      opacity = 0.f;

    std::shared_ptr<Texture> texture; //!< Tileset texture
};

//! \brief RenderOrder stream output operator
std::ostream& operator<< (std::ostream&, TileRenderOrder);

//!@{ RenderOrder string conversions
bool try_parse (const std::string&, TileRenderOrder&);
std::string to_string (TileRenderOrder);
//!@}

} // namespace rdge
