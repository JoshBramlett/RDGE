//! \headerfile <rdge/assets/tilemap/layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/11/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/math/vec2.hpp>

//!@{ Forward declarations
//namespace nlohmann { class json; }
// TODO Add forward declaration
// https://github.com/nlohmann/json/issues/314
#include <rdge/util/json.hpp>
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace tilemap {

//! \enum LayerType
//! \brief Base layer type
enum class LayerType
{
    INVALID = -1,
    TILELAYER,   //!< Maps tiles from a \ref Tileset
    OBJECTGROUP, //!< Layer of objects, which may be sprites and/or collision data
    IMAGELAYER,  //!< Layer made up of a single image
    GROUP        //!< Layer which groups together multiple layers
};

//! \class Layer
//! \brief Generic tilemap layer
//! \details Layers of a tilemap are similar to layers of a Photoshop image;  They
//!          are rendered in an order from bottom to top.  This is a generic container
//!          that whose behavior is defined by the \ref LayerType, and could
//!          potentially be void of rendering data altogether (i.e. a layer which
//!          defines collision rectangles).
//!
//!          The proprietary json format expands on the Tiled default format.
//!
//! - Properties common to all objects
//! \code{.json}
//! {
//!   "name": "my_name",
//!   "opacity": 0.5,
//!   "visible": true,
//!   "offsetx": 0,
//!   "offsety": 10.0,
//!   "properties": [ ... ]
//! }
//! \endcode
//!
//! - \ref LayerType::TILELAYER
//! \code{.json}
//! {
//!   "type": "tilelayer",
//!   "startx": 0,
//!   "starty": 32,
//!   "width": 4,
//!   "height": 4,
//!   "data": [1, 2, 1, 2, 3, 1, 3, 1, 2, 2, 3, 3, 4, 4, 4, 1],
//!   "chunks": [ {
//!     "x": 0,
//!     "y": 0,
//!     "width": 4,
//!     "height": 4,
//!     "data": [ ... ],
//!   } ]
//! }
//! \endcode
//!
//! - \ref LayerType::OBJECTGROUP
//! \code{.json}
//! {
//!   "type": "objectgroup",
//!   "draworder": "topdown",
//!   "objects": [ ... ]
//! }
//! \endcode
//!
//! - \ref LayerType::IMAGELAYER
//! \code{.json}
//! {
//!   "type": "imagelayer",
//!   "image": "/path/to/file.png"
//! }
//! \endcode
//!
//! - \ref LayerType::GROUP
class Layer
{
public:
    //! \brief Layer ctor
    //! \param [in] j json formatted layer
    //! \throws rdge::Exception Parsing failed
    Layer (const nlohmann::json& j);

    //!@{ Layer default ctor/dtor
    Layer (void) = default;
    ~Layer (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    Layer (const Layer&) = delete;
    Layer& operator= (const Layer&) = delete;
    Layer (Layer&&) noexcept = default;
    Layer& operator= (Layer&&) noexcept = default;
    //!@}

public:
    LayerType type = LayerType::INVALID; //!< Base type
    int32 tileset_index = -1;            //!< Index of the tileset in the tilemap

    //!@{ Custom identifiers
    std::string name; //!< Name assigned in editor
    //!@}

    //!@{ Rendering/physics properties
    math::vec2 offset; //!< x/y offset (in pixels) from the origin
    float opacity;     //!< Normalized layer opacity
    bool  visible;     //!< Layer is shown in editor
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

// breaks unit tests
//private:
    //friend class rdge::TileLayer;

    //!@{ LayerType::TILELAYER
    struct tile_chunk
    {
        int32 x;                  //!< x-coordinate in the tile grid
        int32 y;                  //!< y-coordinate in the tile grid
        std::vector<uint32> data; //!< Array of GIDs
    };

    math::ivec2 grid_location;
    math::uivec2 grid_size;
    std::vector<tile_chunk> chunks; //!< List of chunks that make up the mapping
    //!@}

    //!@{ LayerType::OBJECTGROUP
    std::vector<Object> objects;               //!< Array of objects
    DrawOrder draw_order = DrawOrder::INVALID; //!< How to render the layer
    //!@}

    // image

    // group
    std::vector<Layer> layers;
};

//! \brief LayerType stream output operator
std::ostream& operator<< (std::ostream&, LayerType);

} // namespace tilemap

//!@{ LayerType string conversions
bool try_parse (const std::string&, tilemap::LayerType&);
std::string to_string (tilemap::LayerType);
//!@}

} // namespace rdge
