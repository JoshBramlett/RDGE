#include <rdge/graphics/layers/layer.hpp>

namespace RDGE {
namespace Graphics {

Layer::Layer (std::unique_ptr<Shader> shader, RDGE::Math::mat4 projection_matrix)
    : m_shader(std::move(shader))
    , m_projectionMatrix(projection_matrix)
{ }

Layer::~Layer (void)
{ }

Layer::Layer (Layer&& rhs) noexcept
    : m_shader(std::move(rhs.m_shader))
    , m_projectionMatrix(rhs.m_projectionMatrix)
{ }

Layer&
Layer::operator= (Layer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_shader = std::move(rhs.m_shader);
        m_projectionMatrix = rhs.m_projectionMatrix;
    }

    return *this;
}

} // namespace Graphics
} // namespace RDGE
