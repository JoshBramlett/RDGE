#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>

namespace rdge {
namespace physics {

RigidBody::RigidBody (const rigid_body_profile& profile, CollisionGraph* parent)
    : graph(parent)
    , user_data(profile.user_data)
    , linear_velocity(profile.linear_velocity)
    , angular_velocity(profile.angular_velocity)
    , linear_damping(profile.linear_damping)
    , angular_damping(profile.angular_damping)
    , gravity_scale(profile.gravity_scale)
    , m_transform(profile.position, profile.angle)
    , m_type(profile.type)
{
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

    m_sweep.pos_0 = m_transform.pos;
    m_sweep.pos_n = m_transform.pos;
    m_sweep.angle_0 = profile.angle;
    m_sweep.angle_n = profile.angle;

    if (m_type == RigidBodyType::DYNAMIC)
    {
        m_mass = 1.f;
        m_invMass = 1.f;
    }
}

Fixture*
RigidBody::CreateFixture (const fixture_profile& profile)
{
    // TODO IsLocked

    void* cursor = graph->block_allocator.Alloc(sizeof(Fixture));
    Fixture* result = new (cursor) Fixture(profile, this);

    result->body = this;
    result->next = this->fixtures;
    this->fixtures = result;
    fixture_count++;

    if (result->density > 0.f)
    {
        ComputeMass();
    }

    return result;
}

void
RigidBody::DestroyFixture (Fixture* /* fixture */)
{
    // TODO
}

void
RigidBody::ComputeMass (void)
{
    m_mass = 0.f;
    m_invMass = 0.f;
    m_inertia = 0.f;
    m_invInertia = 0.f;
    m_sweep.local_center = { 0.f, 0.f };

    if (m_type == RigidBodyType::STATIC || m_type == RigidBodyType::KINEMATIC)
    {
        m_sweep.pos_0 = m_transform.pos;
        m_sweep.pos_n = m_transform.pos;
        m_sweep.angle_0 = m_sweep.angle_n;
        return;
    }

    math::vec2 local_center;
    for (Fixture* f = this->fixtures; f != nullptr; f = f->next)
    {
        if (f->density == 0.f)
        {
            continue;
        }

        mass_data md = f->GetMassData();
        m_mass += md.mass;
        local_center += md.mass * md.centroid;
        m_inertia += md.mmoi;
    }

    if (m_mass > 0.f)
    {
        m_invMass = 1.f / m_mass;
        local_center *= m_invMass;
    }
    else
    {
        m_mass = 1.f;
        m_invMass = 1.f;
    }

    if (m_inertia > 0.f && (m_flags & FIXED_ROTATION_FLAG) == 0u)
    {
        m_inertia -= m_mass * local_center.self_dot();
        SDL_assert(m_inertia > 0.f);
        m_invInertia = 1.f / m_inertia;
    }
    else
    {
        m_inertia = 0.f;
        m_invInertia = 0.f;
    }

    math::vec2 old_center = m_sweep.pos_n;
    m_sweep.local_center = local_center;
    m_sweep.pos_0 = m_sweep.pos_n * m_transform.rot.rotate(m_sweep.local_center);

    this->linear_velocity += (m_sweep.pos_n - old_center).perp() * this->angular_velocity;
}

} // namespace physics
} // namespace rdge
