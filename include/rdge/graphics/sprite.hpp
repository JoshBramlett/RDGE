//! \headerfile <rdge/graphics/irenderable2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/irenderable2d.hpp>
#include <rdge/gfx/color.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>
#include <vector>

class Srite : IRenderable2D
{
public:
    math::vec3 position;
    math::vec2 size;
    rdge::gfx::color color;
    std::shared_ptr<rdge::Texture> texture;
    std::vector<math::vec2> uv;

    std::array<vertex_attribute, 4> Vertices (void)
    {
        m_vertices[0].pos = /* xform */ this->position;
        m_vertices[0].uv = this->uv[0];
        m_vertices[0].tid = this->texture->UnitID();
        m_vertices[0].color = static_cast<uint32>(this->color);

        m_vertices[1].pos = /* xform */ math::vec3(this->position.x,
                                                   this->position.y + this->size.y,
                                                   this->position.z);
        m_vertices[1].uv = this->uv[1];
        m_vertices[1].tid = this->texture->UnitID();
        m_vertices[1].color = static_cast<uint32>(this->color);

        m_vertices[2].pos = /* xform */ math::vec3(this->position.x + this->size.x,
                                                   this->position.y + this->size.y,
                                                   this->position.z);

        m_vertices[2].uv = this->uv[2];
        m_vertices[2].tid = this->texture->UnitID();
        m_vertices[2].color = static_cast<uint32>(this->color);

        m_vertices[3].pos = /* xform */ math::vec3(this->position.x + this->size.x,
                                                   this->position.y,
                                                   this->position.z);
        m_vertices[3].uv = this->uv[3];
        m_vertices[3].tid = this->texture->UnitID();
        m_vertices[3].color = static_cast<uint32>(this->color);

        return m_vertices;
    }

private:
    std::array<vertex_attributes, 4> m_vertices;
};




// Scene
//  --> Layer
//       --> SpriteBatch
//            --> Shader
//  --> Layer
//       --> SpriteBatch
//  --> Layer
//       --> SpriteBatch
//
// 1)  Layers and SpriteBatch needs to be decoupled.  A layer should be able to use an
//     existing SpriteBatch.
// 2)  Layer in it's new form really only has a single responsibility, which is to
//     keep the appropriate z-index of the sprites it manages.  I could extend a layer
//     to have behavior, like ParallaxLayer, which would manage the movement of the
//     sprite it manages.
// 3)  If multiple layers reference the same SpriteBatch, it'll need to be known when
//     the SpriteBatch is created the appropriate size.
//
// Object pools
// 1)  drawable_attributes?  Should a sprite be created from the SpriteBatch, which
//     assigns a cursor from a pool of drawable attributes?  The benefit could be
//     that rendering would be faster b/c it could iterate through the pool which
//     means contiguous memeory.  The sprites themselves must be able to be referenced
//     outside, (to update position, etc).  Also, I'm thinking a sprite be the main
//     container for game entities.  Meaning the player should be a sprite, but of course
//     be referenced even outside the scene.  If chrono enters a cave, and a new scene is
//     pushed on the stack, his values should persist.

