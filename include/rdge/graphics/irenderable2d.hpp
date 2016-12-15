//! \headerfile <rdge/graphics/irenderable2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2016

#pragma once

#include <rdge/core.hpp>
//#include <rdge/math/vec2.hpp>
//#include <rdge/math/vec3.hpp>
//#include <rdge/gfx/color.hpp>

//#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \class IRenderable2D
//! \brief Interface for 2D renderables
class IRenderable2D
{
public:
    //! \brief IRenderable2D dtor
    virtual ~IRenderable2D (void) noexcept = default;

    // Maybe?
    //virtual std::vector<vertex_attributes> Vertices (void) = 0;
    // Maybe not?
    //virtual math::vec3 Position (void) = 0;
    //virtual rdge::vec2 Size (void) = 0;
    //virtual std::vector<rdge::math::vec2> UV (void) = 0;
    //virtual rdge::int32 TextureUnitID (void) = 0;
    //virtual rdge::color Color (void) = 0;

    //! \brief Called within the event polling phase of the game loop
    //! \details Each entity is responsible for reacting to update events
    //! \param [in] event Polled SDL_Event
    //virtual void HandleEvents (const rdge::Event& event) = 0;
};

} // namespace rdge
