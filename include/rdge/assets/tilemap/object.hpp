//! \headerfile <rdge/assets/tilemap/object.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/tilemap/property.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/aabb.hpp>
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
    AABB,
    CIRCLE,
    POLYGON,
    POLYLINE,
    TEXT
};

//! \class Object
//! \brief Generic tilemap object
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
    physics::aabb GetAABB (void) const;
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
    float      rotation; //!< Angle in degrees clockwise
    bool       visible;  //!< Object is shown in editor
    //!@}

    PropertyCollection properties; //!< Custom variable type property collection

private:
    int32      m_gid = 0;
    math::vec2 m_size;

    physics::polygon::PolygonData m_vertices;
    size_t                        m_numVerts = 0;

    // TODO text support
};

//! \brief ObjectType stream output operator
std::ostream& operator<< (std::ostream&, ObjectType);

} // namespace tilemap

//! \brief ObjectType string conversion
std::string to_string (tilemap::ObjectType);

} // namespace rdge
