//! \headerfile <rdge/assets/tilemap/layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/11/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/object.hpp>
#include <rdge/assets/tilemap/property.hpp>
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
    TILE,
    OBJECT,
    IMAGE,
    GROUP
};

//! \class Layer
//! \brief Generic tilemap layer
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

    //!@{ Renderable layer generation
    //TileLayer GenerateTileLayer (void) const;
    //SpriteLayer GenerateSpriteLayer (void) const;
    //!@}

public:
    LayerType type = LayerType::INVALID; //!< Base type

    //!@{ Custom identifiers
    std::string name; //!< Name assigned in editor
    //!@}

    //!@{ Rendering/physics properties
    math::vec2 offset; //!< x/y offset from the origin
    float opacity;     //!< Normalized layer opacity
    bool  visible;     //!< Object is shown in editor
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

private:
    // tilelayer
    std::vector<int32> data; //!< Array of GIDs
    int32 rows = 0;          //!< Row count (same as map for fixed-sized maps)
    int32 cols = 0;          //!< Column count (same as map for fixed-sized maps)

    // object group
    enum object_draw_order
    {
        object_draw_topdown,
        object_draw_index
    };

    std::vector<Object> objects;
    object_draw_order draw_order;

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
