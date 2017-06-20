#include <rdge/physics/contact.hpp>
#include <rdge/physics/collision_graph.hpp>
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
}

void
Contact::Update (GraphListener* listener)
{
    m_flags |= ENABLED;

    bool was_touching = IsTouching();
    bool is_touching = false;

    auto shape_a = fixture_a->GetWorldShape();
    auto shape_b = fixture_b->GetWorldShape();

    bool is_sensor = fixture_a->IsSensor() || fixture_b->IsSensor();
    if (is_sensor)
    {
        manifold.count = 0;
        is_touching = shape_a->intersects_with(shape_b);
    }
    else
    {
        is_touching = shape_a->intersects_with(shape_b, manifold);

        // TODO ??? Don't really understand Box2D here.  They compare the previous
        //      contacts with the current before storing the impulses.  From the
        //      Box2D comments:
        //      "Match old contact ids to new contact ids and copy the
        //      stored impulses to warm start the solver."

        if (was_touching != is_touching)
        {
            fixture_a->body->WakeUp();
            fixture_b->body->WakeUp();
        }
    }

    if (is_touching)
    {
        m_flags |= TOUCHING;
    }
    else
    {
        m_flags &= ~TOUCHING;
    }

    if (listener)
    {
        if (is_touching && (was_touching == false))
        {
            listener->OnContactStart(this);
        }

        if (was_touching && (is_touching == false))
        {
            listener->OnContactEnd(this);
        }

        if (is_touching && (is_sensor == false))
        {
            // TODO Presolve
        }
    }
}

} // namespace physics
} // namespace rdge
