//! \headerfile <rdge/graphics/sprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/08/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/color.hpp>

#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Sprite
//! \brief Represents a 2D renderable sprite
class Sprite : public Renderable2D
{
public:
    //! \brief Sprite ctor
    //! \param [in] x x-coordinate location
    //! \param [in] y y-coordinate location
    //! \param [in] width Width of the sprite
    //! \param [in] height Height of the sprite
    //! \param [in] color Color of all vertices
    explicit Sprite (float x, float y, float width, float height, const RDGE::Color& color);

    //! \brief Sprite ctor
    //! \param [in] x x-coordinate location
    //! \param [in] y y-coordinate location
    //! \param [in] width Width of the sprite
    //! \param [in] height Height of the sprite
    //! \param [in] texture Texture the sprite will use
    //! \param [in] uv UV coordinates
    explicit Sprite (
                     float x, float y,
                     float width, float height,
                     std::shared_ptr<GLTexture> texture,
                     UVCoordinates uv = DefaultUVCoordinates()
                    );
};

} // namespace Graphics
} // namespace RDGE
