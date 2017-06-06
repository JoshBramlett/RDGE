#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

namespace rdge {
namespace physics {

using namespace rdge::math;

RigidBody::RigidBody (const rigid_body_profile& profile, CollisionGraph* parent)
    : graph(parent)
    , user_data(profile.user_data)
    , world_transform(profile.position, profile.angle)
    , gravity_scale(profile.gravity_scale)
    , m_type(profile.type)
{
    linear.velocity = profile.linear_velocity;
    linear.damping = profile.linear_damping;
    angular.velocity = profile.angular_velocity;
    angular.damping = profile.angular_damping;

    if (profile.simulate)
    {
        m_flags |= SIMULATE;
    }

    if (profile.awake)
    {
        m_flags |= AWAKE;
    }

    if (profile.prevent_rotation)
    {
        m_flags |= PREVENT_ROTATION;
    }

    if (profile.prevent_sleep)
    {
        m_flags |= PREVENT_SLEEP;
    }

    if (profile.bullet)
    {
        m_flags |= BULLET;
    }

    m_sweep.pos_0 = world_transform.pos;
    m_sweep.pos_n = world_transform.pos;
    m_sweep.angle_0 = profile.angle;
    m_sweep.angle_n = profile.angle;

    if (m_type == RigidBodyType::DYNAMIC)
    {
        linear.mass = 1.f;
        linear.inv_mass = 1.f;
    }
}

RigidBody::~RigidBody (void) noexcept
{
    fixtures.for_each([=](auto* f) {
        graph->UnregisterProxy(f->proxy);
        graph->block_allocator.Delete<Fixture>(f);
    });

    contact_edges.for_each([=](auto* edge) {
        // body should never remove edges from it's list.  Doing so would
        // leave the other body with a dangling pointer.
        graph->DestroyContact(edge->contact);
    });

    // TODO destroy joints
}

Fixture*
RigidBody::CreateFixture (const fixture_profile& profile)
{
    if (graph->IsLocked())
    {
        SDL_assert(false);
        return nullptr;
    }

    Fixture* result = graph->block_allocator.New<Fixture>(profile, this);
    fixtures.push_back(result);

    if (IsSimulating())
    {
        result->proxy->box = world_transform.to_world(result->ComputeAABB());
        result->proxy->handle = graph->RegisterProxy(result->proxy);
    }

    if (result->density > 0.f)
    {
        ComputeMass();
    }

    return result;
}

void
RigidBody::DestroyFixture (Fixture* fixture)
{
    SDL_assert(fixture);
    SDL_assert(fixture->body == this);
    SDL_assert(fixtures.contains(fixture));

    if (graph->IsLocked())
    {
        SDL_assert(false);
        return;
    }

    if (IsSimulating())
    {
        graph->UnregisterProxy(fixture->proxy);
        fixture->proxy->handle = fixture_proxy::INVALID_HANDLE;
    }

    contact_edges.for_each([=](auto* edge) {
        Contact* c = edge->contact;
        if (fixture == c->fixture_a || fixture == c->fixture_b)
        {
            graph->DestroyContact(c);
        }
    });

    fixtures.remove(fixture);
    graph->block_allocator.Delete<Fixture>(fixture);

    ComputeMass();
}

bool
RigidBody::HasEdge (const Fixture* a, const Fixture* b) noexcept
{
    SDL_assert(a && b);
    SDL_assert(a->body == this || b->body == this);
    if (b->body == this)
    {
        std::swap(a, b);
    }

    bool result = false;
    contact_edges.for_each([=, &result](auto* edge) {
        if (edge->other == b->body)
        {
            Contact* c = edge->contact;
            if ((c->fixture_a == a && c->fixture_b == b) ||
                (c->fixture_a == b && c->fixture_b == a))
            {
                result = true;
                return;
            }
        }
    });

    return result;
}

void
RigidBody::SyncProxies (void)
{
    // From Box2D
    // Compute an AABB that covers the swept shape (may miss some rotation effect)

    auto sweep_start = m_sweep.lerp_transform(0.f);
    auto displacement = world_transform.pos - sweep_start.pos;
    fixtures.for_each([&](auto* f) {
        aabb box = f->ComputeAABB();
        aabb box_0 = sweep_start.to_world(box);
        aabb box_n = world_transform.to_world(box);

        f->proxy->box = aabb::merge(box_0, box_n);
        graph->MoveProxy(f->proxy, displacement);
    });
}

void
RigidBody::ComputeMass (void)
{
    linear.mass = 0.f;
    linear.inv_mass = 0.f;
    angular.mmoi = 0.f;
    angular.inv_mmoi = 0.f;
    m_sweep.local_center = { 0.f, 0.f };

    if (m_type == RigidBodyType::STATIC || m_type == RigidBodyType::KINEMATIC)
    {
        m_sweep.pos_0 = world_transform.pos;
        m_sweep.pos_n = world_transform.pos;
        m_sweep.angle_0 = m_sweep.angle_n;
        return;
    }

    mass_data body_mass;
    fixtures.for_each([&](auto* f) {
        if (f->density != 0.f)
        {
            mass_data fixture_mass = f->ComputeMass();
            body_mass.mass += fixture_mass.mass;
            body_mass.centroid += fixture_mass.mass * fixture_mass.centroid;
            body_mass.mmoi += fixture_mass.mmoi;
        }
    });

    if (body_mass.mass > 0.f)
    {
        linear.mass = body_mass.mass;
        linear.inv_mass = 1.f / linear.mass;
        body_mass.centroid *= linear.inv_mass;
    }
    else
    {
        linear.mass = 1.f;
        linear.inv_mass = 1.f;
    }

    if (body_mass.mmoi > 0.f && !IsFixedRotation())
    {
        // TODO ??? Why subtract the parallel axis?
        angular.mmoi -= linear.mass * body_mass.centroid.self_dot();
        SDL_assert(angular.mmoi > 0.f);
        angular.inv_mmoi = 1.f / angular.mmoi;
    }
    else
    {
        angular.mmoi = 0.f;
        angular.inv_mmoi = 0.f;
    }

    math::vec2 old_center = m_sweep.pos_n;
    m_sweep.local_center = body_mass.centroid;
    m_sweep.pos_0 = m_sweep.pos_n * world_transform.rot.rotate(m_sweep.local_center);

    // TODO ??? Don't really understand this
    linear.velocity += (m_sweep.pos_n - old_center).perp() * angular.velocity;
}

} // namespace physics
} // namespace rdge
