#include <rdge/physics/contact.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/math/intrinsics.hpp>

#include <SDL_assert.h>

#include <algorithm>

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
    SDL_assert(fixture_a);
    SDL_assert(fixture_b);
    SDL_assert(fixture_a != fixture_b);

    if (fixture_a->shape.world->type() == ShapeType::CIRCLE &&
        fixture_b->shape.world->type() == ShapeType::POLYGON)
    {
        std::swap(this->fixture_a, this->fixture_b);
    }

    this->edge_a.contact = this;
    this->edge_b.contact = this;
    this->edge_a.other = fixture_b->body;
    this->edge_b.other = fixture_a->body;

    if (fixture_a->IsSensor() || fixture_b->IsSensor())
    {
        m_flags |= HAS_SENSOR;
    }
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

    if (m_flags & HAS_SENSOR)
    {
        manifold.count = 0;
        is_touching = shape_a->intersects_with(shape_b);
    }
    else
    {
        is_touching = shape_a->intersects_with(shape_b, manifold);
        SDL_assert(is_touching == shape_a->intersects_with(shape_b));

        if (was_touching != is_touching)
        {
            fixture_a->body->WakeUp();
            fixture_b->body->WakeUp();
        }
    }

    SET_FLAG(is_touching, m_flags, TOUCHING);

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

        if (is_touching && (m_flags & HAS_SENSOR) == 0)
        {
            listener->OnPreSolve(this, old_manifold);
        }
    }
}

} // namespace physics
} // namespace rdge
