//! \headerfile <rdge/graphics/layers/layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.7
//! \date 05/21/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/mat4.hpp>

#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {

//! \class Layer
//! \brief Base class for a layer of renderable objects
//! \details A layer represents a logical group of renderable objects which
//!          will be rendered with the same shader.  Layers can be used to
//!          represent layers in photoshop in order to provide z-index
//!          ordering.  Since each layer has their own shader and renderer,
//!          each can therefore leverage the max textures supported by OpenGL.
class Layer
{
public:
    // TODO: RDGE-00042 Once 3D is supported AddRenderable should be added
    //                  either as a virtual or pure virtual using the base
    //                  class as the parameter.  Consider changing Render
    //                  to simply a virtual.

    virtual void ProcessEventPhase (rdge::Event& event) = 0;

    virtual void ProcessUpdatePhase (rdge::uint32 ticks) = 0;

    //! \brief Render all cached renderables
    virtual void Render (void) = 0;

protected:
    //! \brief Layer ctor
    //! \param [in] shader Shader the layer will take ownership of
    //! \param [in] projection_matrix Projection matrix
    explicit Layer (std::unique_ptr<Shader> shader, rdge::math::mat4 projection_matrix);

    //! \brief Layer dtor
    virtual ~Layer (void);

    //! \brief Layer Copy ctor
    //! \details Non-copyable
    Layer (const Layer&) = delete;

    //! \brief Layer Move ctor
    //! \details Transfers ownership
    Layer (Layer&&) noexcept;

    //! \brief Layer Copy Assignment Operator
    //! \details Non-copyable
    Layer& operator= (const Layer&) = delete;

    //! \brief Layer Move Assignment Operator
    //! \details Transfers ownership
    Layer& operator= (Layer&&) noexcept;

protected:
    std::unique_ptr<Shader> m_shader;
    rdge::math::mat4        m_projectionMatrix;
};

} // namespace rdge
