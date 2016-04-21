//! \headerfile <rdge/graphics/gltexture.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/surface.hpp>

#include <memory>
#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

// TODO: There's two ways to implement activating a texture within OpenGL
//           1) Activate when the texture is registered with the renderer.
//              The major con is that all textures to be registered must
//              be generated (assigned a texture id) prior to activating them
//              (assigning a shader unit id).
//           2) Activate during render (flush) phase.
//              This is safer and allows textures to be created at any time,
//              however, this takes up cycles during the most performance
//              critical time.  Also, activation is performed every frame,
//              whereas in option 1 it's only done once.
//
//       Currently, option 2 is implemented.
//
//       Also, there is no bind/unbind b/c there's no reason (at this time) to
//       allow a caller to do that.  All state updates should be done within
//       the class which can call bind/unbind itself.
//
//       Also, the shader should not be bind/unbind.  It should be install or use
//       (no unbind req).


class GLTexture : public std::enable_shared_from_this<GLTexture>
{
public:
    explicit GLTexture (const std::string& file);

    explicit GLTexture (RDGE::Surface& surface);

    ~GLTexture (void);

    std::shared_ptr<GLTexture> GetSharedPtr (void)
    {
        return shared_from_this();
    }

    RDGE::UInt32 Width (void) const noexcept { return m_width; }

    RDGE::UInt32 Height (void) const noexcept { return m_height; }

    RDGE::Int32 UnitID (void) const noexcept { return m_textureUnitId; }

    void SetUnitID (RDGE::Int32 id);

    void ResetData (RDGE::Surface& surface);

    void Activate (void) const;

    //void Bind (void);

    //void Unbind (void);

private:
    RDGE::UInt32 m_textureId;
    RDGE::Int32  m_textureUnitId;

    RDGE::UInt32 m_width;
    RDGE::UInt32 m_height;
};

} // namespace Graphics
} // namespace RDGE
