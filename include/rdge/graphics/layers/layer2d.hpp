//! \headerfile <rdge/graphics/layers/layer2d.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/02/2016

#pragma once

#include <rdge/types.hpp>
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
//         1)  The projection matrix is hard-coded.  Either needs to be supplied
//             at construction or allow it to be updated.  The biggest reason it's
//             a problem is b/c it hard codes the aspect ratio
//         2)  The reason for the GetShader() method was to allow an external
//             source to update the shader uniform values.  Either this needs to
//             be a helper method or removed completely.
//         3)  The renderable objects are not managed.  I don't think the layer
//             should own them, and if not to make it truly safe I'd have to use
//             weak pointers
//         4)  Should this class have it's own z-index setting?  It would make
//             sense, and should overwrite the z value in the renderable
//             objects positions.
//         5)  The ctor could also accept a max # of renderables so the vector
//             doessn't have to re-allocate when the sprite count is high.
//         6)  This and the renderer are so intertwined with the shader code
//             that the shader code really should be moved internal to the layer
//             and the shader should no longer be a parameter in the ctor

//! \class Layer2D
//! \brief
class Layer2D
{
public:
    //! \brief Layer2D ctor
    //! \param [in] shader Shader the layer will take ownership of
    Layer2D (std::unique_ptr<Shader> shader);

    virtual ~Layer2D (void);

    virtual void AddRenderable (Renderable2D* renderable);

    virtual void Render (void);

    // TODO: Remove?
    Shader* GetShader (void)
    {
        return m_shader.get();
    }

private:
    Renderer2D                 m_renderer;
    std::vector<Renderable2D*> m_renderables;
    std::unique_ptr<Shader>    m_shader;
    RDGE::Math::mat4           m_projectionMatrix;
};

} // namespace Graphics
} // namespace RDGE
