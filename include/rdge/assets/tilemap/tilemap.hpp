//! \headerfile <rdge/assets/tilemap/tilemap.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/22/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/color.hpp>
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

//! \enum Orientation
//! \brief Tilemap rendering orientation
enum class Orientation
{
    INVALID = -1,
    ORTHOGONAL,
    ISOMETRIC,
    STAGGERED,
    HEXAGONAL
};

//! \enum RenderOrder
//! \brief The order in which tiles are rendered
//! \details Useful when tiles require a certain z-indexing.  Only available with
//!          orthogonal maps.  In all cases, the map is drawn row-by-row
//! \see https://github.com/bjorn/tiled/issues/455
enum class RenderOrder
{
    INVALID = -1,
    RIGHT_DOWN,
    RIGHT_UP,
    LEFT_DOWN,
    LEFT_UP
};

//! \class Tilemap
//! \brief Generic tilemap layer
class Tilemap
{
public:
    //! \brief Tilemap ctor
    //! \param [in] j json formatted layer
    //! \throws rdge::Exception Parsing failed
    Tilemap (const nlohmann::json& j);

    //!@{ Tilemap default ctor/dtor
    Tilemap (void) = default;
    ~Tilemap (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    Tilemap (const Tilemap&) = delete;
    Tilemap& operator= (const Tilemap&) = delete;
    Tilemap (Tilemap&&) noexcept = default;
    Tilemap& operator= (Tilemap&&) noexcept = default;
    //!@}

    //!@{ Base type accessors
    math::vec2 GetPoint (void) const;
    physics::aabb GetAABB (void) const;
    physics::circle GetCircle (void) const;
    physics::polygon GetPolygon (void) const;
    //!@}

public:

    //!@{ Rendering properties
    color background = color::BLACK;
    Orientation orientation = Orientation::INVALID;
    RenderOrder render_order = RenderOrder::INVALID;
    //!@}

    //!@{ Grid properties
    math::vec2 cell_size; //!< Map grid cell size
    size_t rows = 0;      //!< Number of rows in the grid
    size_t cols = 0;      //!< Number of columns in the grid
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

    std::vector<Layer> layers;
    std::vector<Tileset> tilesets;
};

//! \brief Orientation stream output operator
std::ostream& operator<< (std::ostream&, Orientation);

//! \brief RenderOrder stream output operator
std::ostream& operator<< (std::ostream&, RenderOrder);

} // namespace tilemap

//!@{ Orientation string conversions
bool try_parse (const std::string&, tilemap::Orientation&);
std::string to_string (tilemap::Orientation);
//!@}

//!@{ RenderOrder string conversions
bool try_parse (const std::string&, tilemap::RenderOrder&);
std::string to_string (tilemap::RenderOrder);
//!@}

} // namespace rdge
