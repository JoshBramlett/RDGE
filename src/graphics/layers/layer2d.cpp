#include <rdge/graphics/layers/layer2d.hpp>
#include <rdge/math/functions.hpp>

namespace RDGE {
namespace Graphics {

Layer2D::Layer2D (
                  std::unique_ptr<Shader> shader,
                  RDGE::Math::mat4        projection_matrix,
                  float                   zindex,
                  RDGE::UInt16            num_renderables
                 )
    : Layer(std::move(shader), projection_matrix)
    , m_renderer(num_renderables)
    , m_zIndex(RDGE::Math::clamp(zindex, 0.0f, 1.0f))
{
    // For initializing our sampler2D array
    std::vector<RDGE::Int32> texture_units(Shader::MaxFragmentShaderUnits());
    RDGE::Int32 n = {0};
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    // Reserve memory for our vector of renderables (does nothing if less than current capacity)
    m_renderables.reserve(num_renderables);

    m_shader->Enable();
    m_shader->SetUniformValue("pr_matrix", m_projectionMatrix);
    m_shader->SetUniformValue("textures", texture_units.size(), texture_units.data());
    m_shader->Disable();
}

Layer2D::~Layer2D (void)
{ }

Layer2D::Layer2D (Layer2D&& rhs) noexcept
    : Layer(std::move(rhs))
    , m_renderer(std::move(rhs.m_renderer))
    , m_renderables(std::move(rhs.m_renderables))
    , m_zIndex(rhs.m_zIndex)
{ }

Layer2D&
Layer2D::operator= (Layer2D&& rhs) noexcept
{
    if (this != &rhs)
    {
        Layer::operator=(std::move(rhs));
        m_renderer = std::move(rhs.m_renderer);
        m_renderables = std::move(rhs.m_renderables);
        m_zIndex = rhs.m_zIndex;
    }

    return *this;
}

void
Layer2D::AddRenderable (std::shared_ptr<Renderable2D> renderable)
{
    auto texture = renderable->Texture();
    if (texture)
    {
        m_renderer.RegisterTexture(texture);
    }

    renderable->SetZIndex(m_zIndex);
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
