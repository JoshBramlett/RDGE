#include <rdge/physics/contact.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/math/intrinsics.hpp>

namespace rdge {
namespace physics {

namespace {

float mix_friction (float a, float b)
{
    return std::sqrtf(a * b);
}

constexpr float mix_restitution (float a, float b)
{
    return std::max(a, b);
}

Contact::ManifoldGenerationFn s_circleCircle = [](ishape* a, ishape* b) {
    circle* ca = static_cast<circle*>(a);
    circle* cb = static_cast<circle*>(b);

    // TODO Needs the transform
    collision_manifold mf;
    ca->intersects_with(*cb, mf);

    return mf;
};

} // anonymous namespace

Contact::Contact (Fixture* a, Fixture* b)
    : fixture_a(a)
    , fixture_b(b)
    , friction(mix_friction(a->friction, b->friction))
    , restitution(mix_restitution(a->restitution, b->restitution))
{
    edge_a.contact = this;
    edge_b.contact = this;
    edge_a.other = b->body;
    edge_b.other = a->body;

    if (a->shape->type() == ShapeType::CIRCLE && b->shape->type() == ShapeType::CIRCLE)
    {
        m_evaluate = s_circleCircle;
    }
}

void
Contact::Update (ContactListener* listener)
{
    m_flags |= ENABLED;

    bool was_touching = IsTouching();
    bool is_touching = false;

    bool is_sensor = fixture_a->IsSensor() || fixture_b->IsSensor();
    if (is_sensor)
    {

    }
    else
    {

    }

    if (listener)
    {
        if (was_touching == false && is_touching == true)
        {
            listener->BeginContact(this);
        }

        if (was_touching == true && is_touching == false)
        {
            listener->EndContact(this);
        }
    }
}

bool
ContactFilter::ShouldCollide (Fixture* a, Fixture* b) const noexcept
{
    if (a->filter.group && a->filter.group == b->filter.group)
    {
        return (a->filter.group > 0);
    }

    return (a->filter.mask & b->filter.category) &&
           (b->filter.mask & a->filter.category);
}

} // namespace physics
} // namespace rdge
