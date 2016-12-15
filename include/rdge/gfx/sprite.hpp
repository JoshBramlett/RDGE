//! \headerfile <rdge/gfx/sprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/08/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/gfx/renderable2d.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/gfx/color.hpp>
#include <rdge/math/vec2.hpp>

#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

//! \struct sprite_uv
//! \brief UV coordinates for a sprite
struct sprite_uv
{
    rdge::math::vec2 top_left;
    rdge::math::vec2 top_right;
    rdge::math::vec2 bottom_left;
    rdge::math::vec2 bottom_right;
};

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
    explicit Sprite (float x, float y, float width, float height, const color& color);

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
                     std::shared_ptr<rdge::Texture> texture,
                     UVCoordinates uv = DefaultUVCoordinates()
                    );
};

} // namespace gfx
} // namespace rdge
