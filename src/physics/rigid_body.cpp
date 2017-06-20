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

    sweep.pos_0 = world_transform.pos;
    sweep.pos_n = world_transform.pos;
    sweep.angle_0 = profile.angle;
    sweep.angle_n = profile.angle;

    if (m_type == RigidBodyType::DYNAMIC)
    {
        linear.mass = 1.f;
        linear.inv_mass = 1.f;
    }
}

RigidBody::~RigidBody (void) noexcept
{
    fixtures.for_each([=](auto* f) {
        graph->block_allocator.Delete<Fixture>(f);
    });
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
RigidBody::SyncFixtures (void)
{
    // From Box2D
    // Compute an AABB that covers the swept shape (may miss some rotation effect)

    // TODO This should not only sync the proxy aabb for the broad phase,
    //      but also sync the shape in world coordinates.  Basically any time
    //      the transform is updated the fixtures should be synced

    auto sweep_start = sweep.lerp_transform(0.f);
    auto displacement = world_transform.pos - sweep_start.pos;
    fixtures.for_each([&](auto* f) {
        aabb box = f->ComputeAABB();
        aabb box_0 = sweep_start.to_world(box);
        aabb box_n = world_transform.to_world(box);

        f->proxy->box = aabb::merge(box_0, box_n);
        graph->MoveProxy(f->proxy, displacement);

        f->Syncronize();
    });
}

void
RigidBody::ComputeMass (void)
{
    linear.mass = 0.f;
    linear.inv_mass = 0.f;
    angular.mmoi = 0.f;
    angular.inv_mmoi = 0.f;
    sweep.local_center = { 0.f, 0.f };

    if (m_type == RigidBodyType::STATIC || m_type == RigidBodyType::KINEMATIC)
    {
        sweep.pos_0 = world_transform.pos;
        sweep.pos_n = world_transform.pos;
        sweep.angle_0 = sweep.angle_n;
        return;
    }

    math::vec2 local_center(0.f, 0.f);
    fixtures.for_each([&](auto* f) {
        if (f->density != 0.f)
        {
            mass_data fixture_mass = f->ComputeMass();
            linear.mass += fixture_mass.mass;
            angular.mmoi += fixture_mass.mmoi;
            local_center += fixture_mass.mass * fixture_mass.centroid;
        }
    });

    if (linear.mass > 0.f)
    {
        linear.inv_mass = 1.f / linear.mass;
        local_center *= linear.inv_mass;
    }
    else
    {
        linear.mass = 1.f;
        linear.inv_mass = 1.f;
    }

    if (angular.mmoi > 0.f && !IsFixedRotation())
    {
        // TODO ??? Why subtract the parallel axis?
        angular.mmoi -= linear.mass * local_center.self_dot();
        SDL_assert(angular.mmoi > 0.f);
        angular.inv_mmoi = 1.f / angular.mmoi;
    }
    else
    {
        angular.mmoi = 0.f;
        angular.inv_mmoi = 0.f;
    }

    math::vec2 old_center = sweep.pos_n;
    sweep.local_center = local_center;
    sweep.pos_0 = sweep.pos_n * world_transform.rot.rotate(sweep.local_center);

    // TODO ??? Don't really understand this
    linear.velocity += (sweep.pos_n - old_center).perp() * angular.velocity;
}

} // namespace physics
} // namespace rdge
