//! \headerfile <rdge/graphics/texture.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/12/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <limits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Surface;
struct shared_texture_data;
//!@}

//! \class Texture
//! \brief Shared wrapper for an OpenGL texture object
//! \details A light weight wrapper around a shared OpenGL texture.  Instances of
//!          the wrapper are keyed off of the \ref Surface data to avoid uploading
//!          duplicate textures to OpenGL, and will increment an internal ref count
//!          which manages when to unload the texture data from GPU memory.  The
//!          wrapper provides a texture unit id member that is not shared amongst
//!          instances and can be set per render target.
//! \note Maintaining scope on the underlying \ref Surface is not required, but to
//!       take advantage of the shared textures all instances must be created
//!       with the same Surface.
class Texture
{
public:
    //! \var INVALID_UNIT_ID Constant defining the texture unit id is not set
    static constexpr uint32 INVALID_UNIT_ID = std::numeric_limits<uint32>::max();

    //! \brief Texture default ctor
    Texture (void) noexcept = default;

    //! \brief Texture ctor
    //! \details Loads the texture asset from a file.  The pixel data (\ref Surface)
    //!          is not managed and will be destroyed after texture is created.
    //! \param [in] path Path of the underlying asset to the texture
    //! \throws rdge::Exception Surface cannot be loaded
    explicit Texture (const std::string& path);

    //! \brief Texture ctor
    //! \details Load the texture from an existing asset.
    //! \param [in] surface Underlying surface asset
    //! \note The surface pixel format may be changed to accommodate OpenGL
    explicit Texture (Surface& surface);
    explicit Texture (Surface&& surface);

    //! \brief Texture dtor
    //! \details Deletes the texture from the OpenGL context
    ~Texture (void) noexcept;

    //!@{ Copy and move enabled
    Texture (const Texture&);
    Texture& operator= (const Texture&);
    Texture (Texture&&) noexcept;
    Texture& operator= (Texture&&) noexcept;
    //!@}

    //!@{ Basic Surface properties
    bool IsEmpty (void) const noexcept;
    bool IsUnique (void) const noexcept;
    size_t Width (void) const noexcept;
    size_t Height (void) const noexcept;
    math::uivec2 Size (void) const noexcept;
    //!@}

    //! \brief Activate and binds the texture to the OpenGL context
    //! \throws rdge::Exception Invalid sampler unit id
    void Activate (void) const;

    //! \brief Reset the pixel data of the texture
    //! \details Update texture and discard the previous stored data.
    //! \param [in] surface Underlying surface asset
    //! \note The surface pixel format may be changed to accommodate OpenGL
    void Reload (Surface& surface);


    // TODO - from libgdx
    // https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/GLTexture.java
    // SetFilter
    // SetWrap
    // Texture.java also has mapped enums
public:
    uint32 unit_id = INVALID_UNIT_ID; //!< Shader sampler2D texture unit Id

private:
    shared_texture_data* m_data = nullptr;
};

} // namespace rdge
