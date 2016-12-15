#include <rdge/gfx/layers/group.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>
#include <limits>
#include <algorithm>

using namespace rdge;
using namespace rdge::gfx;
using namespace rdge::math;

Group::Group (const rdge::math::mat4& transformation)
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

    // TODO: This confused me (and took a bit of debugging to figure out)
    //       but m_size represents the right and bottom, rather than the
    //       size.  This should be changed.

    //std::cout << "left=" << left << std::endl
              //<< "right=" << right << std::endl
              //<< "top=" << top << std::endl
              //<< "bottom=" << bottom << std::endl;

    m_position.x = std::min(m_position.x, left);
    m_position.y = std::min(m_position.y, top);
    m_size.x = std::max(m_size.x, right);
    m_size.y = std::max(m_size.y, bottom);

    //std::cout << "left=" << m_position.x << std::endl
              //<< "right=" << m_size.x << std::endl
              //<< "top=" << m_position.y << std::endl
              //<< "bottom=" << m_size.y << std::endl;

    m_children.push_back(renderable);

    // TODO:
    //
    // Fuck.  I don't know what the final coordinates will be for a group
    // because only the position is cached, and the transformation has not
    // yet been done.  I guess I could calculate the transformation every
    // handle events call, but that seems crazy inefficient.
    //
    // TODO:
    // It only makes sense that renderables calculate their transformations
    // themselves rather than it being done by the renderer.  They also should
    // be cached in order to properly ray cast.  Renderable should have another
    // method called GetDrawingPosition().
}

void
Group::SetTransformation (const rdge::math::mat4& transformation)
{
    m_transformation = transformation;
}

void
Group::RotateOnCenter (float angle)
{
    // TODO: Good candidate for a helper method.  No need to make this group specific

    m_transformation *= mat4::translation(vec3(m_size.x / 2.0f, m_size.y / 2.0f, 0.0f));
    m_transformation *= mat4::rotation(angle, vec3(0.0f, 0.0f, 1.0f));
    m_transformation *= mat4::translation(vec3(-m_size.x / 2.0f, -m_size.y / 2.0f, 0.0f));
}

void
Group::Submit (Renderer2D* renderer) const
{
    renderer->PushTransformation(m_transformation);

    for (auto& child : m_children)
    {
        child->Submit(renderer);
    }

    renderer->PopTransformation();
}

void
Group::SetOpacity (rdge::uint8 opacity)
{
    for (auto& child : m_children)
    {
        child->SetOpacity(opacity);
    }
}

void
Group::SetOpacity (float opacity)
{
    auto clamped = rdge::math::clamp(opacity, 0.0f, 1.0f);
    auto value = static_cast<rdge::uint8>(clamped * 255);

    for (auto child : m_children)
    {
        child->SetOpacity(value);
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

void
Group::RegisterTextures (Renderer2D* renderer) const
{
    for (auto child : m_children)
    {
        auto texture = child->Texture();
        if (texture)
        {
            renderer->RegisterTexture(texture);
        }
    }
}
