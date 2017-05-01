#include <rdge/physics/rigid_body.hpp>

namespace rdge {
namespace physics {

RigidBody::RigidBody (const rigid_body_profile& profile);
    : user_data(profile.user_data)
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

} // namespace physics
} // namespace rdge
