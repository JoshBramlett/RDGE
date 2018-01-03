//! \headerfile <rdge/assets/tilemap/object.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>

//!@{ Forward declarations
//namespace nlohmann { class json; }
// TODO Add forward declaration
// https://github.com/nlohmann/json/issues/314
#include <rdge/util/json.hpp>
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace tilemap {

//! \enum ObjectType
//! \brief Base object type
enum class ObjectType
{
    INVALID = -1,
    SPRITE,
    POINT,
    CIRCLE,
    POLYGON,
    POLYLINE,
    TEXT
};

//! \class Object
//! \brief Generic tilemap object
//! \details A tilemap object can be of any supported \ref ObjectType, generally
//!          representing sprites or collision shapes.  All types share certain
//!          properties, but each type has a distinct proprietary json format:
//!
//! - Properties common to all objects
//! \code{.json}
//! {
//!   "id": 36,
//!   "name": "my_name",
//!   "type": "my_type",
//!   "x": 952.894,
//!   "y": 648.108,
//!   "visible": true,
//!   "properties": [ ... ]
//! }
//! \endcode
//!
//! - \ref ObjectType::SPRITE
//! \code{.json}
//! {
//!   "obj_type": "sprite",
//!   "gid": 5,
//!   "width": 32,
//!   "height": 32,
//!   "rotation": 0.0
//! }
//! \endcode
//!
//! - \ref ObjectType::POINT
//! \code{.json}
//! {
//!   "obj_type": "point"
//! }
//! \endcode
//!
//! - \ref ObjectType::CIRCLE
//! \code{.json}
//! {
//!   "obj_type": "circle",
//!   "radius": 15.5
//! }
//! \endcode
//!
//! - \ref ObjectType::POLYGON
//! \code{.json}
//! {
//!   "obj_type": "polygon",
//!   "coords": [
//!     {
//!       "x": 0.0,
//!       "y": 0.0
//!     },
//!     ...
//!   ]
//! }
//! \endcode
//!
//! - \ref ObjectType::POLYLINE
//!   - Currently unsupported
//!
//! - \ref ObjectType::TEXT
//!   - Currently unsupported
//!
class Object
{
public:
    //! \brief Object ctor
    //! \param [in] j json formatted object
    //! \throws rdge::Exception Parsing failed
    Object (const nlohmann::json& j);

    //!@{ Object default ctor/dtor
    Object (void) = default;
    ~Object (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    Object (const Object&) = delete;
    Object& operator= (const Object&) = delete;
    Object (Object&&) noexcept = default;
    Object& operator= (Object&&) noexcept = default;
    //!@}

    //!@{ Base type accessors
    math::vec2 GetPoint (void) const;
    physics::circle GetCircle (void) const;
    physics::polygon GetPolygon (void) const;
    //!@}

public:
    ObjectType type = ObjectType::INVALID; //!< Base type

    //!@{ Custom identifiers
    int32       id = -1;     //!< Globally unique id
    std::string name;        //!< 'Name' field assigned in editor
    std::string custom_type; //!< 'Type' field assigned in editor
    //!@}

    //!@{ Rendering/physics properties
    math::vec2 position; //!< Position relative to parent
    bool       visible;  //!< Object is shown in editor
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

private:
    //!@{ Sprite properties
    int32      m_gid = 0;        //!< Tilemap texture region Id
    math::vec2 m_size;           //!< Sprite size
    float      m_rotation = 0.f; //!< Angle in degrees clockwise
    //!@}

    //!@{ Polygon properties
    float m_radius = 0.f;
    //!@}

    //!@{ Polygon properties
    physics::polygon::PolygonData m_vertices;
    size_t                        m_numVerts = 0;
    //!@}
};

//! \brief ObjectType stream output operator
std::ostream& operator<< (std::ostream&, ObjectType);

} // namespace tilemap

//!@{ ObjectType string conversions
bool try_parse (const std::string&, tilemap::ObjectType&);
std::string to_string (tilemap::ObjectType);
//!@}

} // namespace rdge
