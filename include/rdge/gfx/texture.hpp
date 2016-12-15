//! \headerfile <rdge/gfx/texture.hpp>
//! \author Josh Bramlett
//! \version 0.0.8
//! \date 06/07/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>

#include <memory>
#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

//! \class Texture
//! \brief OpenGL texture object
//! \details The texture is registered in OpenGL and therefore given a texture
//!          id, but in addition the texture maps directly to a fragment shader
//!          sampler unit.  This assumes the shader has an array of sampler2D
//!          objects, and the unit id of the texture is the index in the array.
class Texture : public std::enable_shared_from_this<Texture>
{
public:
    //! \brief Texture ctor
    //! \details Default ctor.
    Texture (void);

    //! \brief Texture ctor
    //! \details Loads the image asset directly and the underlying asset will be
    //!          destroyed after initialization (safe once the pixel data is
    //!          copied to OpenGL).
    //! \param [in] file File of the underlying asset to the texture
    //! \throws RDGE::Exception surface cannot be loaded
    explicit Texture (const std::string& file);

    //! \brief Texture ctor
    //! \details The surface is non-const as the pixel format may need to be
    //!          modified to comply with the OpenGL format.
    //! \param [in] surface Shared pointer to the underlying asset
    explicit Texture (std::shared_ptr<rdge::Surface>& surface);

    //! \brief Texture dtor
    //! \details Deletes the texture object from OpenGL
    virtual ~Texture (void);

    //! \brief Safely retrieve additional shared_ptr instance
    //! \returns Shared pointer of the current object
    //! \throws std::bad_weak_ptr If called when object is not managed
    //!         by a shared_ptr
    std::shared_ptr<Texture> GetSharedPtr (void) noexcept
    {
        return shared_from_this();
    }

    //! \brief Get the width of the texture
    //! \returns Texture width
    rdge::uint32 Width (void) const noexcept { return m_width; }

    //! \brief Get the height of the texture
    //! \returns Texture height
    rdge::uint32 Height (void) const noexcept { return m_height; }

    //! \brief Get the registered shader sampler2D index
    //! \details Value is initialized to -1, which is required by the renderer
    //!          to determine whether to register the texture with OpenGL, or
    //!          if the texture is already registered.  When registering, it's
    //!          the responsibility of the renderer to assign an appropriate
    //!          sampler unit id.
    //! \returns Index of the shader sampler2D array
    rdge::int32 UnitID (void) const noexcept { return m_textureUnitId; }

    //! \brief Set the sampler unit index
    //! \details Sets which index the fragment shader's sampler2D array the
    //!          texture is mapped to.
    //! \param [in] id Index of shader's sampler2D array
    void SetUnitID (rdge::int32 id);

    //! \brief Map the bound texture to the OpenGL texture unit
    //! \throws RDGE::Exception Invalid sampler unit id
    void Activate (void) const;

    //! \brief Reset the pixel data of the texture
    //! \details Capies the pixel data of the provided surface to OpenGL.
    //! \param [in] surface Shared pointer to the underlying asset
    void ResetData (std::shared_ptr<rdge::Surface>& surface);

protected:
    rdge::uint32 m_textureId;
    rdge::int32  m_textureUnitId;

    // width and height are cached because the underlying asset may be destroyed
    rdge::uint32 m_width;
    rdge::uint32 m_height;
};

} // namespace gfx
} // namespace rdge
