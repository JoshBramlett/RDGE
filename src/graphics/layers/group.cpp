#include <rdge/graphics/layers/group.hpp>

#include <memory>

namespace RDGE {
namespace Graphics {

Group::Group (const RDGE::Math::mat4& transformation)
    : m_transformation(transformation)
{ }

Group::Group (Group&& rhs) noexcept
    : m_children(std::move(rhs.m_children))
    , m_transformation(std::move(rhs.m_transformation))
{
    rhs.m_children.clear();
}

Group&
Group::operator= (Group&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_children = std::move(rhs.m_children);
        m_transformation = std::move(rhs.m_transformation);

        rhs.m_children.clear();
    }

    return *this;
}

void
Group::AddRenderable (Renderable2D* renderable)
{
    m_children.push_back(renderable);
}

void
Group::Submit (Renderer2D* renderer) const
{
    renderer->PushTransformation(m_transformation);

    for (auto child : m_children)
    {
        child->Submit(renderer);
    }

    renderer->PopTransformation();
}

} // namespace Graphics
} // namespace RDGE
