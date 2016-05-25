#include <rdge/graphics/layers/group.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>
#include <limits>
#include <algorithm>

namespace RDGE {
namespace Graphics {

Group::Group (const RDGE::Math::mat4& transformation)
    : m_transformation(transformation)
{
    m_position.x = std::numeric_limits<float>::max();
    m_position.y = std::numeric_limits<float>::max();
}

Group::Group (Group&& rhs) noexcept
    : m_children(std::move(rhs.m_children))
    , m_transformation(rhs.m_transformation)
{ }

Group&
Group::operator= (Group&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_children = std::move(rhs.m_children);
        m_transformation = rhs.m_transformation;
    }

    return *this;
}

void
Group::AddRenderable (std::shared_ptr<Renderable2D> renderable)
{
    // Calculate the position and total size of all children
    auto pos = renderable->Position();
    auto size = renderable->Size();

    auto left = pos.x;
    auto top = pos.y;
    auto right = pos.x + size.x;
    auto bottom = pos.y + size.y;

    m_position.x = std::min(m_position.x, left);
    m_position.y = std::min(m_position.y, top);
    m_size.x = std::max(m_size.x, right);
    m_size.y = std::max(m_size.y, bottom);

    m_children.push_back(renderable);
}

void
Group::SetTransformation (const RDGE::Math::mat4& transformation)
{
    m_transformation = transformation;
}

void
Group::RotateOnCenter (float angle)
{
    using namespace RDGE::Math;

    m_transformation *= mat4::translation(vec3(m_size.x / 2.0f, m_size.y / 2.0f, 0.0f));
    m_transformation *= mat4::rotation(angle, vec3(0.0f, 0.0f, 1.0f));
    m_transformation *= mat4::translation(vec3(-m_size.x / 2.0f, -m_size.y / 2.0f, 0.0f));
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

void
Group::SetOpacity (RDGE::UInt8 opacity)
{
    for (auto child : m_children)
    {
        child->SetOpacity(opacity);
    }
}

void
Group::SetZIndex (float zindex)
{
    for (auto child : m_children)
    {
        child->SetZIndex(zindex);
    }
}

} // namespace Graphics
} // namespace RDGE
