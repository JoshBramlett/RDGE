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

Fixture*
RigidBody::CreateFixture (const fixture_profile& profile)
{
    // TODO IsLocked

    Fixture* result = graph->block_allocator.New<Fixture>(profile, this);
    fixtures.push_back(result);

    if (result->density > 0.f)
    {
        ComputeMass();
    }

    // TODO fixture->CreateProxies(...)
    //      Issue I have with this is the need to pass the broad phase to the
    //      fixture.  Let's try to keep classes as segregated as possible.
    //
    // TODO m_world->m_flags |= b2World::e_newFixture;

    return result;
}

void
RigidBody::DestroyFixture (Fixture* fixture)
{
    // TODO IsLocked

    fixtures.remove(fixture);

    // TODO fixture->DestroyProxies(...)

    graph->block_allocator.Delete<Fixture>(fixture);
    ComputeMass();
}

void
RigidBody::ComputeMass (void)
{
    this->linear.mass = 0.f;
    this->linear.inv_mass = 0.f;
    this->angular.mmoi = 0.f;
    this->angular.inv_mmoi = 0.f;
    m_sweep.local_center = { 0.f, 0.f };

    if (m_type == RigidBodyType::STATIC || m_type == RigidBodyType::KINEMATIC)
    {
        m_sweep.pos_0 = world_transform.pos;
        m_sweep.pos_n = world_transform.pos;
        m_sweep.angle_0 = m_sweep.angle_n;
        return;
    }

    math::vec2 local_center;
    fixtures.for_each([this, &local_center](Fixture* f) {
        if (f->density == 0.f)
        {
            return;
        }

        mass_data md = f->ComputeMass();
        this->linear.mass += md.mass;
        local_center += md.mass * md.centroid;
        this->angular.mmoi += md.mmoi;
    });

    if (this->linear.mass > 0.f)
    {
        this->linear.inv_mass = 1.f / this->linear.mass;
        local_center *= this->linear.inv_mass;
    }
    else
    {
        this->linear.mass = 1.f;
        this->linear.inv_mass = 1.f;
    }

    if (this->angular.mmoi > 0.f && (m_flags & FIXED_ROTATION_FLAG) == 0u)
    {
        this->angular.mmoi -= this->linear.mass * local_center.self_dot();
        SDL_assert(this->angular.mmoi > 0.f);
        this->angular.inv_mmoi = 1.f / this->angular.mmoi;
    }
    else
    {
        this->angular.mmoi = 0.f;
        this->angular.inv_mmoi = 0.f;
    }

    math::vec2 old_center = m_sweep.pos_n;
    m_sweep.local_center = local_center;
    m_sweep.pos_0 = m_sweep.pos_n * world_transform.rot.rotate(m_sweep.local_center);

    this->linear.velocity += (m_sweep.pos_n - old_center).perp() * this->angular.velocity;
}

} // namespace physics
} // namespace rdge
