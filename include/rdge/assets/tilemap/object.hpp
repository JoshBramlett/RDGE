//! \headerfile <rdge/assets/tilemap/object.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/shapes/polygon.hpp>

//!@{ Forward declarations
#include <nlohmann/json_fwd.hpp>
namespace rdge {
namespace physics { struct circle; }
namespace tilemap { class Tilemap; }
}
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

//! \struct extended_object_data
//! \brief Custom object traits shared between objects
//! \details The \ref Tilemap manages an array of all extended data which
//!          can be accessed by the object's custom_type property
struct extended_object_data
{
    std::string name;              //!< Unique name
    color color;                   //!< Debug wireframe color
    PropertyCollection properties; //!< Shared properties
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
    //! \param [in] parent
    //! \throws rdge::Exception Parsing failed
    Object (const nlohmann::json& j, Tilemap* parent = nullptr);

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

    //! \brief Check if the object represents a primitive shape.
    bool IsFixture (void) const noexcept
    {
        return this->type == ObjectType::CIRCLE ||
               this->type == ObjectType::POLYGON ||
               this->type == ObjectType::POLYLINE;
    }

    //!@{ Base type accessors
    math::vec2 GetPoint (float scale = 1.f) const;
    physics::circle GetCircle (float scale = 1.f) const;
    physics::polygon GetPolygon (float scale = 1.f, bool local = false) const;
    //!@}

public:
    ObjectType type = ObjectType::INVALID; //!< Base type

    //!@{ Custom identifiers
    int32 id = -1;           //!< [unused] Globally unique id
    std::string name;        //!< 'Name' field assigned in editor
    //!@}

    //!@{ Rendering/physics properties
    math::vec2 pos; //!< Position relative to parent
    bool visible;   //!< Object is shown in editor
    //!@}

    //!@{
    // \brief Shared cusotm object attributes
    // \details Extended data is optional.  If the object was constructed
    //          from the \ref Tilemap the pointer will be set.  If the object
    //          was constructed outside the Tilemap (e.g. SpriteSheet) you
    //          can query the Tilemap using the ext_type.
    std::string ext_type;
    const extended_object_data* ext_data = nullptr;
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

    Tilemap* parent = nullptr; //!< Circular reference to parent (if avavilable)

public:
    //!@{ ObjectType::SPRITE
    struct object_sprite_data
    {
        uint32 gid = 0;       //!< Texture region Id (offset by 1, zero is undefined)
        math::vec2 size;      //!< [unused] Sprite size
        float rotation = 0.f; //!< Angle in degrees clockwise
    } sprite;
    //!@}

    //!@{ ObjectType::CIRCLE
    struct object_circle_data
    {
        float radius = 0.f;
    } circle;
    //!@}

    //!@{ ObjectType::POLYGON
    struct object_polygon_data
    {
        physics::polygon::PolygonData vertices;
        size_t vertex_count = 0;
    } polygon;
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
