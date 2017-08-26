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

    auto old_manifold = manifold;
    bool was_touching = IsTouching();
    bool is_touching = false;

    auto shape_a = fixture_a->shape.world;
    auto shape_b = fixture_b->shape.world;

    bool is_sensor = fixture_a->IsSensor() || fixture_b->IsSensor();
    if (is_sensor)
    {
        manifold.count = 0;
        is_touching = shape_a->intersects_with(shape_b);
    }
    else
    {
        is_touching = shape_a->intersects_with(shape_b, manifold);
        if (is_touching)
        {
            // TODO This needs to be cleaned up.  Box2D does all collision processing
            //      in local space, but I had set it up to process in global space.
            //      The solver however does an iterative method for position correction
            //      with a new transform computed each step, so solving in local
            //      coordinates seems the right way to go.  The following is just a
            //      hack so the solver can do it's thing.
            //
            // TODO make sure circle/polygon collision is correct.
            iso_transform* ref;
            iso_transform* inc;

            if (manifold.flip)
            {
                ref = &fixture_b->body->world_transform;
                inc = &fixture_a->body->world_transform;
            }
            else
            {
                ref = &fixture_a->body->world_transform;
                inc = &fixture_b->body->world_transform;
            }

            manifold.local_normal = ref->rot.inv_rotate(manifold.normal);
            manifold.local_plane = ref->to_local(manifold.local_plane);
            for (size_t i = 0; i < manifold.count; i++)
            {
                manifold.local_contacts[i] = inc->to_local(manifold.contacts[i]);
            }
        }

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
        if (is_touching && !was_touching)
        {
            listener->OnContactStart(this);
        }

        if (was_touching && !is_touching)
        {
            listener->OnContactEnd(this);
        }

        if (is_touching && !is_sensor)
        {
            listener->OnPreSolve(this, old_manifold);
        }
    }
}

} // namespace physics
} // namespace rdge
