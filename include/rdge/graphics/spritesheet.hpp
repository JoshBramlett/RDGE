//! \headerfile <rdge/graphics/spritesheet.hpp>
//! \author Josh Bramlett
//! \version 0.0.8
//! \date 06/07/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/assets/surface.hpp>

#include <memory>
#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class SpriteSheet
//! \brief OpenGL texture containing uv data for multiple sprites
//! \details UV data for each individual sprite is read from a json text file
//!          and can be queried through the interface by name or index.
//! \code{.json}
//! {
//!     "file": "textures/image.png",
//!     "uv": [ {
//!         "index": 0,
//!         "name": "MY_UV_1",
//!         "values": [ [0, 0.5], [0, 0], [0.0625, 0], [0.0625, 0.5] ]
//!     } ]
//! }
class SpriteSheet : public GLTexture
{
public:
    //! \brief SpriteSheet ctor
    //! \details Loads the image asset as defined by the json file, and
    //!          caches the uv data.
    //! \param [in] config json configuration defining the sprite sheet
    //! \throws RDGE::Exception Unable to parse config
    explicit SpriteSheet (const std::string& config);

    //! \brief SpriteSheet dtor
    //! \details Deletes the texture object from OpenGL
    virtual ~SpriteSheet (void) { }

    //! \brief Safely retrieve additional shared_ptr instance
    //! \returns Shared pointer of the current object
    //! \throws std::bad_weak_ptr If called when object is not managed
    //!         by a shared_ptr
    std::shared_ptr<SpriteSheet> GetSharedPtr (void) noexcept
    {
        return std::static_pointer_cast<SpriteSheet>(shared_from_this());
    }

    //! \brief Query for UV coordinates by index
    //! \details Lookup is performed on the json config
    //! \param [in] index Numeric index as defined in the config
    //! \throws RDGE::Exception Index not in collection
    const UVCoordinates& LookupUV (RDGE::Int32 index);

    //! \brief Query for UV coordinates by name
    //! \details Lookup is performed on the json config
    //! \param [in] name Name as defined in the config
    //! \throws RDGE::Exception Name not in collection
    const UVCoordinates& LookupUV (const std::string& name);

private:
    //! \struct sheet_element
    //! \brief POD to hold cached imported data
    struct sheet_element
    {
        RDGE::Int32   index;
        std::string   name;
        UVCoordinates uv;
    };

    std::vector<sheet_element> m_elements;
};

} // namespace Graphics
} // namespace RDGE
