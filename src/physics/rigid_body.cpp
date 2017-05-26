#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>

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
    this->linear.velocity = profile.linear_velocity;
    this->linear.damping = profile.linear_damping;
    this->angular.velocity = profile.angular_velocity;
    this->angular.damping = profile.angular_damping;

    if (profile.active)
    {
        m_flags |= ACTIVE_FLAG;
    }

    if (profile.awake)
    {
        m_flags |= AWAKE_FLAG;
    }

    if (profile.bullet)
    {
        m_flags |= BULLET_FLAG;
    }

    if (profile.fixed_rotation)
    {
        m_flags |= FIXED_ROTATION_FLAG;
    }

    if (profile.allow_sleep)
    {
        m_flags |= AUTOSLEEP_FLAG;
    }

    m_sweep.pos_0 = world_transform.pos;
    m_sweep.pos_n = world_transform.pos;
    m_sweep.angle_0 = profile.angle;
    m_sweep.angle_n = profile.angle;

    if (m_type == RigidBodyType::DYNAMIC)
    {
        this->linear.mass = 1.f;
        this->linear.inv_mass = 1.f;
    }
}

RigidBody::~RigidBody (void) noexcept
{
    fixtures.for_each([this](auto* f) {
        graph->UnregisterProxy(f->proxy);
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

    if (IsActive())
    {
        graph->RegisterProxy(result->proxy);
    }

    if (result->density > 0.f)
    {
        ComputeMass();
    }

    // TODO m_world->m_flags |= b2World::e_newFixture;

    return result;
}

void
RigidBody::DestroyFixture (Fixture* fixture)
{
    SDL_assert(fixture);
    SDL_assert(fixture->body == this);

    if (graph->IsLocked())
    {
        SDL_assert(false);
        return;
    }

    if (IsActive())
    {
        graph->UnregisterProxy(fixture->proxy);
    }

    // TODO remove all contacts associated with the fixture

    fixtures.remove(fixture);
    graph->block_allocator.Delete<Fixture>(fixture);

    ComputeMass();
}

void
RigidBody::SynchronizeFixtures (void)
{
    // TODO ??? Don't really understand how this works.  Code below breaks behavior.
    //      The only comment from Box2D:
    //
    // Compute an AABB that covers the swept shape (may miss some rotation effect).
    //
    //      Important note is the fixture proxy is merged with both the
    //      start and end of the sweep.  I was considering simply storing the
    //      fat aabb in the proxy to avoid the duplicate, but I'll need to
    //      revisit that when I implement the broad phase.
    //
    //      Also this makes me wonder if my overall understanding was wrong.  I thought
    //      the sweep step would basically be reset every frame, but I'm not sure
    //      that's the case anymore.
    rotation q(m_sweep.angle_0);
    math::vec2 p = m_sweep.pos_0 - q.rotate(m_sweep.local_center);
    iso_transform sweep_start(p, q);

    auto displacement = world_transform.pos - sweep_start.pos;
    fixtures.for_each([&](auto* f) {
        aabb box = f->shape->compute_aabb();
        aabb box_0 = sweep_start.to_world(box);
        aabb box_n = world_transform.to_world(box);

        f->proxy->box.merge(box_0).merge(box_n);

        Unused(displacement);
        //broadPhase->MoveProxy(proxy->proxyId, proxy->aabb, displacement);
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
