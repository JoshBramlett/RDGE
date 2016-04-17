#include <rdge/graphics/layers/layer2d.hpp>

namespace RDGE {
namespace Graphics {

Layer2D::Layer2D (std::unique_ptr<Shader> shader)
    : m_shader(std::move(shader))
    , m_projectionMatrix(RDGE::Math::mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f))
{
    m_shader->Enable();
    m_shader->SetUniformMat4("pr_matrix", m_projectionMatrix);
    m_shader->Disable();
}

Layer2D::~Layer2D (void)
{ }

void
Layer2D::AddRenderable (Renderable2D* renderable)
{
    m_renderables.push_back(renderable);
}

void
Layer2D::Render (void)
{
    m_shader->Enable();
    m_renderer.PrepSubmit();

    for (auto renderable : m_renderables)
    {
        renderable->Submit(&m_renderer);
    }

    m_renderer.EndSubmit();
    m_renderer.Flush();
}

} // namespace Graphics
} // namespace RDGE
