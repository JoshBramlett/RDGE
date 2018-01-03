//! \headerfile <rdge/assets/tilemap/property.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/06/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/util/io/rwops_base.hpp>

//!@{ Forward declarations
//namespace nlohmann { class json; }
// TODO Add forward declaration
// https://github.com/nlohmann/json/issues/314
#include <rdge/util/json.hpp>
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace tilemap {

//! \class PropertyCollection
//! \brief Container for name lookups of variant types
class PropertyCollection
{
public:
    //! \brief PropertyCollection ctor
    //! \param [in] j json formatted property list
    //! \throws rdge::Exception Parsing failed
    PropertyCollection (const nlohmann::json& j);

    //!@{ PropertyCollection default ctor/dtor
    PropertyCollection (void) = default;
    ~PropertyCollection (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    PropertyCollection (const PropertyCollection&) = delete;
    PropertyCollection& operator= (const PropertyCollection&) = delete;
    PropertyCollection (PropertyCollection&&) noexcept = default;
    PropertyCollection& operator= (PropertyCollection&&) noexcept = default;
    //!@}

    //!@{
    //! \brief Property accessors
    //! \throws rdge::Exception Type mismatch
    //! \throws rdge::Exception Lookup failed
    std::string GetString (const std::string& name) const;
    color GetColor (const std::string& name) const;
    rwops_base GetFile (const std::string& name, const char* mode) const;
    float GetFloat (const std::string& name) const;
    int32 GetInt (const std::string& name) const;
    bool GetBool (const std::string& name) const;
    //!@}

    size_t Size (void) const noexcept;

private:

    enum property_type
    {
        property_type_invalid,
        property_type_bool,
        property_type_color,
        property_type_file,
        property_type_float,
        property_type_int,
        property_type_string
    };

    struct property
    {
        std::string name;
        property_type type;

        std::string s;
        int32       i;
        float       f;
    };

    const property& Lookup (const std::string&, property_type) const;

    std::vector<property> m_properties;
};

} // namespace tilemap
} // namespace rdge
