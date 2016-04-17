//! \headerfile <rdge/graphics/gltexture.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/surface.hpp>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

class GLTexture
{
public:
    explicit GLTexture (const std::string& file, RDGE::UInt32 unit);

    explicit GLTexture (RDGE::Surface& surface, RDGE::UInt32 unit);

    ~GLTexture (void);

    void Bind (void);

    void Unbind (void);
private:
    void Init (RDGE::Surface& surface);

    RDGE::UInt32 m_textureId;
    RDGE::UInt32 m_textureUnit;
};

} // namespace Graphics
} // namespace RDGE
