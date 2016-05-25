//! \headerfile <rdge/graphics/layers/layer2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.7
//! \date 05/21/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/layers/layer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/math/mat4.hpp>

#include <vector>
#include <memory>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

// TODO: A couple issues with this class:
//         1)  The reason for the GetShader() method was to allow an external
//             source to update the shader uniform values.  Either this needs to
//             be a helper method or removed completely.
//         2)  The renderable objects are not managed.  I don't think the layer
//             should own them, and if not to make it truly safe I'd have to use
//             weak pointers
//         3)  This and the renderer are so intertwined with the shader code
//             that the shader code really should be moved internal to the layer
//             and the shader should no longer be a parameter in the ctor

//! \class Layer2D
//! \brief Layer of 2D renderables
//! \details Layers have their own shader and renderer, and can therefore manage
//!          the max textures supported by OpenGL.  The renderables are not
//!          managed by the layer, and therefore should should outlive the layer.
class Layer2D : public Layer
{
public:
    //! \brief Layer2D ctor
    //! \param [in] shader Shader the layer will take ownership of
    //! \param [in] projection_matrix Projection matrix
    //! \param [in] zindex Z-Index order
    //! \param [in] num_renderables Used to pre-allocate the renderables vector
    explicit Layer2D (
                      std::unique_ptr<Shader> shader,
                      RDGE::Math::mat4        projection_matrix,
                      float                   zindex          = 1.0f,
                      RDGE::UInt16            num_renderables = 0
                     );

    //! \brief Layer2D dtor
    virtual ~Layer2D (void);

    //! \brief Layer2D Copy ctor
    //! \details Non-copyable
    Layer2D (const Layer2D&) = delete;

    //! \brief Layer2D Move ctor
    //! \details Transfers ownership
    Layer2D (Layer2D&&) noexcept;

    //! \brief Layer2D Copy Assignment Operator
    //! \details Non-copyable
    Layer2D& operator= (const Layer2D&) = delete;

    //! \brief Layer2D Move Assignment Operator
    //! \details Transfers ownership
    Layer2D& operator= (Layer2D&&) noexcept;

    //! \brief Cache a pointer to a renderable to be submitted to the renderer
    //! \param [in] renderable Renderable object shared pointer
    virtual void AddRenderable (std::shared_ptr<Renderable2D> renderable);

    //! \brief Render all cached renderables
    //virtual void Render (void);
    virtual void Render (void) override;

private:
    //! \typedef RenderableVector Container type for layer renderables
    using RenderableVector = std::vector<std::shared_ptr<Renderable2D>>;

    Renderer2D       m_renderer;
    RenderableVector m_renderables;
    float            m_zIndex;
};

} // namespace Graphics
} // namespace RDGE
