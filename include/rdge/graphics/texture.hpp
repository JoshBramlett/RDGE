//! \headerfile <rdge/graphics/texture.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/12/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

#include <GL/glew.h>

#include <limits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class Surface;
struct shared_texture_data;
//!@}

//! \enum TextureFilter
//! \brief Mapping to OpenGL texture filter enum
enum class TextureFilter : uint32
{
    NEAREST                = GL_NEAREST,
    LINEAR                 = GL_LINEAR,
    MIPMAP_NEAREST_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    MIPMAP_LINEAR_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
    MIPMAP_NEAREST_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
    MIPMAP_LINEAR_LINEAR   = GL_LINEAR_MIPMAP_LINEAR,
    MIPMAP                 = MIPMAP_LINEAR_LINEAR       //!< Default mipmap
};

//! \enum TextureWrap
//! \brief Mapping to OpenGL texture wrap enum
//! \see http://www.flipcode.com/archives/Advanced_OpenGL_Texture_Mapping.shtml
enum class TextureWrap : uint32
{
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    REPEAT          = GL_REPEAT
};

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
    //! \details Load the texture from an existing \ref Surface asset.
    //! \param [in] surface Underlying surface asset
    explicit Texture (const Surface& surface);

    //! \brief Texture ctor
    //! \details Load the texture from an existing \ref Surface asset, overriding
    //!          the textures \ref TextureFilter settings.
    //! \param [in] surface Underlying surface asset
    //! \param [in] min Texture minification setting
    //! \param [in] mag Texture magnification setting
    Texture (const Surface& surface, TextureFilter min, TextureFilter mag);

    //! \brief Texture ctor
    //! \details Load the texture from an existing \ref Surface asset, overriding
    //!          the textures \ref TextureWrap settings.
    //! \param [in] surface Underlying surface asset
    //! \param [in] u Wrapping for the u axis
    //! \param [in] v Wrapping for the v axis
    Texture (const Surface& surface, TextureWrap u, TextureWrap v);

    //! \brief Texture ctor
    //! \details Load the texture from an existing \ref Surface asset, overriding
    //!          the textures \ref TextureFilter and \ref TextureWrap settings.
    //! \param [in] surface Underlying surface asset
    //! \param [in] min Texture minification setting
    //! \param [in] mag Texture magnification setting
    //! \param [in] u Wrapping for the u axis
    //! \param [in] v Wrapping for the v axis
    Texture (const Surface& surface,
             TextureFilter min, TextureFilter mag,
             TextureWrap u, TextureWrap v);

    //! \brief Texture dtor
    //! \details Deletes the texture from the OpenGL context
    ~Texture (void) noexcept;

    //!@{ Copy and move enabled
    Texture (const Texture&);
    Texture& operator= (const Texture&);
    Texture (Texture&&) noexcept;
    Texture& operator= (Texture&&) noexcept;
    //!@}

    //!@{ Comparison operators
    bool operator== (const Texture&) const noexcept;
    bool operator!= (const Texture&) const noexcept;
    //!@}

    //!@{ Basic Surface properties
    bool IsEmpty (void) const noexcept;
    bool IsUnique (void) const noexcept;
    size_t Width (void) const noexcept;
    size_t Height (void) const noexcept;
    math::svec2 Size (void) const noexcept;
    //!@}

    //! \brief Set texture filter for minification and magnification
    //! \param [in] min Texture minification setting
    //! \param [in] mag Texture magnification setting
    //! \note This will bind the texture.
    void SetFilter (TextureFilter min, TextureFilter mag);

    //! \brief Set texture wrapping on the u/v axis
    //! \param [in] u Wrapping for the u axis
    //! \param [in] v Wrapping for the v axis
    //! \note This will bind the texture.
    void SetWrap (TextureWrap u, TextureWrap v);

    //! \brief Activate and binds the texture to the OpenGL context
    //! \throws rdge::Exception Invalid sampler unit id
    void Activate (void) const;

private:
    //! \brief Uploads texture data to OpenGL
    void Upload (const Surface&);

public:
    uint32 unit_id = INVALID_UNIT_ID; //!< Shader sampler2D texture unit Id

private:
    shared_texture_data* m_data = nullptr;
};

//! \brief TextureFilter stream output operator
std::ostream& operator<< (std::ostream&, TextureFilter);
//! \brief TextureWrap stream output operator
std::ostream& operator<< (std::ostream&, TextureWrap);

//! \brief TextureFilter string conversion
std::string to_string (TextureFilter);
//! \brief TextureWrap string conversion
std::string to_string (TextureWrap);

} // namespace rdge
