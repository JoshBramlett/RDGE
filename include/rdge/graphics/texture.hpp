//! \headerfile <rdge/graphics/texture.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/12/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>

#include <string>
#include <limits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Texture
//! \brief OpenGL texture object
//! \details A wrapper for an OpenGL texture which manages the handle and the
//!          unit id associated with a fragment shader.
class Texture
{
public:
    //! \var INVALID_UNIT_ID Constant defining the texture unit id is not set
    static constexpr uint32 INVALID_UNIT_ID = std::numeric_limits<uint32>::max();

    //! \brief Texture ctor
    //! \details Default ctor.
    Texture (void);

    //! \brief Texture ctor
    //! \details Loads the texture asset from a file.  The pixel data (\ref Surface)
    //!          is not managed and will be destroyed after texture is created.
    //! \param [in] path Path of the underlying asset to the texture
    //! \throws rdge::Exception Surface cannot be loaded
    explicit Texture (const std::string& path);

    //!@{
    //! \brief Texture ctor
    //! \details Load the texture from an existing asset.
    //! \param [in] surface Underlying surface asset
    //! \note The surface pixel format may be changed to accommodate OpenGL
    explicit Texture (Surface& surface);
    explicit Texture (Surface&& surface);
    //!@}

    //! \brief Texture dtor
    //! \details Deletes the texture from the OpenGL context
    ~Texture (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    Texture (const Texture&) = delete;
    Texture& operator= (const Texture&) = delete;
    Texture (Texture&&) noexcept;
    Texture& operator= (Texture&&) noexcept;
    //!@}

    //! \brief Activate and binds the texture to the OpenGL context
    //! \throws RDGE::Exception Invalid sampler unit id
    void Activate (void) const;

    //! \brief Reset the pixel data of the texture
    //! \details Update texture and discard the previous stored data.
    //! \param [in] surface Underlying surface asset
    //! \note The surface pixel format may be changed to accommodate OpenGL
    void Reload (Surface& surface);

    //! \brief Check if object contains a texture
    //! \returns True if underlying texture is registered, false otherwise
    bool IsEmpty (void) const;

    // TODO - from libgdx
    // https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/GLTexture.java
    // SetFilter
    // SetWrap
    // Texture.java also has mapped enums
public:
    uint32 unit_id = INVALID_UNIT_ID; //!< Shader sampler2D texture unit Id
    uint32 width   = 0;               //!< Cached texture width
    uint32 height  = 0;               //!< Cached texture height

private:
    uint32 m_handle = 0;  //!< OpenGL texture handle
};

} // namespace rdge
