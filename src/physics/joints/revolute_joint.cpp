#include <rdge/physics/joints/revolute_joint.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/solver.hpp>
#include <rdge/physics/isometry.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/mat2.hpp>

namespace rdge {
namespace physics {

// Point-to-point constraint
// C = p2 - p1
// Cdot = v2 - v1
//      = v2 + cross(w2, r2) - v1 - cross(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Motor constraint
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

RevoluteJoint::RevoluteJoint (RigidBody* a, RigidBody* b, const math::vec2& anchor)
    : m_anchor(anchor)
{
    this->body_a = a;
    this->body_b = b;

    SDL_assert(body_a);
    SDL_assert(body_b);
    SDL_assert(body_a != body_b);

    edge_a.joint = this;
    edge_b.joint = this;
    edge_a.other = body_b;
    edge_b.other = body_a;

    m_referenceAngle = body_b->GetAngle() - body_a->GetAngle();
}

float
RevoluteJoint::JointAngle (void) const noexcept
{
	return body_b->GetAngle() - body_a->GetAngle() - m_referenceAngle;
}

float
RevoluteJoint::JointSpeed (void) const noexcept
{
	return body_b->angular.velocity - body_a->angular.velocity;
}

void
RevoluteJoint::EnableMotor (void) noexcept
{
    if ((m_flags & MOTOR_ENABLED) == 0)
    {
		body_a->WakeUp();
		body_a->WakeUp();
        m_flags |= MOTOR_ENABLED;
    }
}

void
RevoluteJoint::DisableMotor (void) noexcept
{
    if (m_flags & MOTOR_ENABLED)
    {
		body_a->WakeUp();
		body_a->WakeUp();
        m_flags &= ~MOTOR_ENABLED;
    }
}

void
RevoluteJoint::SetMotorSpeed (float speed) noexcept
{
	if (speed != m_motorSpeed)
	{
		body_a->WakeUp();
		body_a->WakeUp();
		m_motorSpeed = speed;
	}
}

void
RevoluteJoint::SetMaxMotorTorque (float torque) noexcept
{
	if (torque != m_maxMotorTorque)
	{
		body_a->WakeUp();
		body_a->WakeUp();
		m_maxMotorTorque = torque;
	}
}

void
RevoluteJoint::EnableLimits (void) noexcept
{
    if ((m_flags & LIMIT_ENABLED) == 0)
    {
		body_a->WakeUp();
		body_a->WakeUp();
		m_impulse.z = 0.0f;
        m_flags |= LIMIT_ENABLED;
    }
}

void
RevoluteJoint::DisableLimits (void) noexcept
{
    if (m_flags & LIMIT_ENABLED)
    {
		body_a->WakeUp();
		body_a->WakeUp();
		m_impulse.z = 0.0f;
        m_flags &= ~LIMIT_ENABLED;
    }
}

void
RevoluteJoint::SetLimits (float lower, float upper) noexcept
{
	SDL_assert(lower <= upper);

	if (lower != m_lowerAngle || upper != m_upperAngle)
	{
		body_a->WakeUp();
		body_a->WakeUp();
		m_impulse.z = 0.f;
		m_lowerAngle = lower;
		m_upperAngle = upper;
	}
}

void
RevoluteJoint::InitializeSolver (const time_step&  step,
                                 solver_body_data& bdata_a,
                                 solver_body_data& bdata_b)
{
	// J = [-I -r1_skew I r2_skew]
	//     [ 0       -1 0       1]
	// r_skew = [-ry; rx]

	// Matlab
	// K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
	//     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
	//     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]

    // bodies position relative to the anchor
    math::vec2 rel_point[2];
    rel_point[0] = m_anchor - body_a->GetWorldCenter();
    rel_point[1] = m_anchor - body_b->GetWorldCenter();

    m_mass[0].x = (bdata_a.inv_mass + bdata_b.inv_mass) +
                  (bdata_a.inv_mmoi * math::square(rel_point[0].y)) +
                  (bdata_b.inv_mmoi * math::square(rel_point[1].y));
    m_mass[1].x = (-rel_point[0].y * rel_point[0].x * bdata_a.inv_mmoi) +
                  (-rel_point[1].y * rel_point[1].x * bdata_b.inv_mmoi);
    m_mass[2].x = (-rel_point[0].y * bdata_a.inv_mmoi) +
                  (-rel_point[1].y * bdata_b.inv_mmoi);

    m_mass[0].y = m_mass[1].x;
    m_mass[1].y = (bdata_a.inv_mass + bdata_b.inv_mass) +
                  (bdata_a.inv_mmoi * math::square(rel_point[0].x)) +
                  (bdata_b.inv_mmoi * math::square(rel_point[1].x));
    m_mass[2].y = (rel_point[0].x * bdata_a.inv_mmoi) +
                  (rel_point[1].x * bdata_b.inv_mmoi);

    m_mass[0].z = m_mass[2].x;
    m_mass[1].z = m_mass[2].y;
    m_mass[2].z = (bdata_a.inv_mmoi + bdata_b.inv_mmoi);

    if ((m_flags & MOTOR_ENABLED) == 0)
    {
		m_motorImpulse = 0.0f;
    }
    else
    {
        // cannot have a motor when both bodies have fixed angular velocities
        SDL_assert(bdata_a.inv_mmoi > 0.f || bdata_b.inv_mmoi > 0.f);
    }

	m_motorMass = (bdata_a.inv_mmoi + bdata_b.inv_mmoi);
	if (m_motorMass > 0.f)
	{
		m_motorMass = 1.f / m_motorMass;
	}

    if (m_flags & LIMIT_ENABLED)
    {
        float angle = body_b->GetAngle() - body_a->GetAngle() - m_referenceAngle;
		if (math::abs(m_upperAngle - m_lowerAngle) < (2.f * ANGULAR_SLOP))
        {
            m_limitState = LimitState::EQUAL;
        }
        else if (angle <= m_lowerAngle)
        {
            if (m_limitState != LimitState::AT_LOWER)
            {
                // if not already at the llimit, make limit impulse zero
                m_impulse.z = 0.f;
            }

            m_limitState = LimitState::AT_LOWER;
        }
        else if (angle >= m_upperAngle)
        {
            if (m_limitState != LimitState::AT_UPPER)
            {
                // if not already at the ulimit, make limit impulse zero
                m_impulse.z = 0.f;
            }

            m_limitState = LimitState::AT_UPPER;
        }
        else
        {
            m_limitState = LimitState::INACTIVE;
            m_impulse.z = 0.f;
        }
    }
    else
    {
        m_impulse.z = 0.f;
    }

    // account for the variable time step
    m_impulse *= step.ratio;
    m_motorImpulse *= step.ratio;

    // warm start
    auto impulse = m_impulse.xy();
    float constraint_impulse = m_motorImpulse + m_impulse.z;

    bdata_a.linear_vel -= bdata_a.inv_mass * impulse;
    bdata_a.angular_vel -= bdata_a.inv_mmoi *
                           (math::perp_dot(rel_point[0], impulse) + constraint_impulse);
    bdata_b.linear_vel += bdata_b.inv_mass * impulse;
    bdata_b.angular_vel += bdata_b.inv_mmoi *
                           (math::perp_dot(rel_point[1], impulse) + constraint_impulse);
}

void
RevoluteJoint::SolveVelocityConstraints (const time_step&  step,
                                         solver_body_data& bdata_a,
                                         solver_body_data& bdata_b)
{
	// Solve motor constraint
    if (m_flags & MOTOR_ENABLED && m_limitState != LimitState::EQUAL)
    {
        // get the relative velocity - the target motor speed
        float rv = bdata_b.angular_vel - bdata_a.angular_vel - m_motorSpeed;

        // get the impulse required to obtain the speed
        float impulse = -m_motorMass * rv;

        // clamp the impulse between the maximum torque
        float old_impulse = m_motorImpulse;
        float max_impulse = m_maxMotorTorque * step.dt;
        m_motorImpulse = math::clamp(m_motorImpulse + impulse, -max_impulse, max_impulse);

        // get the impulse we need to apply to the bodies
        impulse = m_motorImpulse - old_impulse;

        bdata_a.angular_vel -= bdata_a.inv_mmoi * impulse;
        bdata_b.angular_vel += bdata_b.inv_mmoi * impulse;
    }

    math::vec2 rel_point[2];
    rel_point[0] = m_anchor - body_a->GetWorldCenter();
    rel_point[1] = m_anchor - body_b->GetWorldCenter();

    math::vec2 vel_a = bdata_a.linear_vel + (rel_point[0].perp() * bdata_a.angular_vel);
    math::vec2 vel_b = bdata_b.linear_vel + (rel_point[1].perp() * bdata_b.angular_vel);

    // the 2x2 version of Jv + b
    math::vec2 jvb2 = vel_b - vel_a;

	// Solve limit constraint
    if (m_flags & LIMIT_ENABLED && m_limitState != LimitState::INACTIVE)
    {
        // solve the point to point constraint including the limit constraint
        float pivot_angular_vel = bdata_b.angular_vel - bdata_a.angular_vel;

        // the 3x3 version of Jv + b
        math::vec3 jvb3(jvb2, pivot_angular_vel);
        math::vec3 impulse = -m_mass.solve(jvb3);

        // check state for how to apply the impulse
        if (m_limitState == LimitState::EQUAL)
        {
            // basically a weld joint - add entire impulse to satisfy the
            // point-to-point and angle constraints
            m_impulse += impulse;
        }
        else if (m_limitState == LimitState::AT_LOWER)
        {
            // clamp the rotational impulse and solve the point-to-point
            // constraint alone
			float new_impulse = m_impulse.z + impulse.z;
			if (new_impulse < 0.f)
			{
                math::vec2 rhs = -jvb2 + math::vec2(m_mass[2].x, m_mass[2].y) * m_impulse.z;
                math::vec2 reduced = m_mass.solve(rhs);
				impulse.x = reduced.x;
				impulse.y = reduced.y;
				impulse.z = -m_impulse.z;
				m_impulse.x += reduced.x;
				m_impulse.y += reduced.y;
				m_impulse.z = 0.f;
			}
			else
			{
				m_impulse += impulse;
			}
        }
        else if (m_limitState == LimitState::AT_UPPER)
        {
            // clamp the rotational impulse and solve the point-to-point
            // constraint alone
			float new_impulse = m_impulse.z + impulse.z;
			if (new_impulse > 0.f)
			{
                math::vec2 rhs = -jvb2 + math::vec2(m_mass[2].x, m_mass[2].y) * m_impulse.z;
                math::vec2 reduced = m_mass.solve(rhs);
				impulse.x = reduced.x;
				impulse.y = reduced.y;
				impulse.z = -m_impulse.z;
				m_impulse.x += reduced.x;
				m_impulse.y += reduced.y;
				m_impulse.z = 0.f;
			}
			else
			{
				m_impulse += impulse;
			}
        }

        math::vec2 xy = impulse.xy();
        bdata_a.linear_vel -= bdata_a.inv_mass * xy;
        bdata_a.angular_vel -= bdata_a.inv_mmoi *
                               math::perp_dot(rel_point[0], xy) + impulse.z;
        bdata_b.linear_vel += bdata_b.inv_mass * xy;
        bdata_b.angular_vel += bdata_b.inv_mmoi *
                               math::perp_dot(rel_point[1], xy) + impulse.z;
    }
	else
	{
		// Solve point-to-point constraint
        math::vec2 impulse = -m_mass.solve(jvb2);
		m_impulse.x += impulse.x;
		m_impulse.y += impulse.y;

        bdata_a.linear_vel -= bdata_a.inv_mass * impulse;
        bdata_a.angular_vel -= bdata_a.inv_mmoi *
                               math::perp_dot(rel_point[0], impulse);
        bdata_b.linear_vel += bdata_b.inv_mass * impulse;
        bdata_b.angular_vel += bdata_b.inv_mmoi *
                               math::perp_dot(rel_point[1], impulse);
	}
}

bool
RevoluteJoint::SolvePositionConstraints (solver_body_data& bdata_a, solver_body_data& bdata_b)
{
    float linear_error = 0.f;
    float angular_error = 0.f;

    if (m_flags & LIMIT_ENABLED && m_limitState != LimitState::INACTIVE)
    {
        float angle = (body_b->GetAngle() + bdata_b.angle) -
                      (body_a->GetAngle() + bdata_a.angle) -
                      m_referenceAngle;
        float limit_impulse = 0.f;

        if (m_limitState == LimitState::EQUAL)
        {
            float c = math::clamp(angle - m_lowerAngle,
                                  -Solver::MAX_ANGULAR_CORRECTION,
                                  Solver::MAX_ANGULAR_CORRECTION);
            limit_impulse = -m_motorMass * c;
            angular_error = math::abs(c);
        }
        else if (m_limitState == LimitState::AT_UPPER)
        {
            float c = angle - m_lowerAngle;
            angular_error = -c;

            c = math::clamp(c + ANGULAR_SLOP, -Solver::MAX_ANGULAR_CORRECTION, 0.f);
            limit_impulse = -m_motorMass * c;
        }
        else if (m_limitState == LimitState::AT_LOWER)
        {
            float c = angle - m_upperAngle;
            angular_error = c;

            c = math::clamp(c - ANGULAR_SLOP, 0.f, Solver::MAX_ANGULAR_CORRECTION);
            limit_impulse = -m_motorMass * c;
        }

        bdata_a.angle -= bdata_a.inv_mmoi * limit_impulse;
        bdata_b.angle += bdata_b.inv_mmoi * limit_impulse;
    }

	// Solve point-to-point constraint.
    auto pos_a = body_a->GetWorldCenter();
    auto pos_b = body_b->GetWorldCenter();

    rotation rot_a(bdata_a.angle);
    rotation rot_b(bdata_b.angle);
    auto r_a = rot_a.rotate(m_anchor - pos_a);
    auto r_b = rot_b.rotate(m_anchor - pos_b);

    auto p = (pos_b + bdata_b.pos + r_b) - (pos_a + bdata_a.pos + r_a);
    linear_error = p.length();

    math::mat2 k;
    k[0].x = (bdata_a.inv_mass + bdata_b.inv_mass) +
             (bdata_a.inv_mmoi * math::square(r_a.y)) +
             (bdata_b.inv_mmoi * math::square(r_b.y));
    k[0].y = -(bdata_a.inv_mmoi * r_a.x * r_a.y) - (bdata_b.inv_mmoi * r_b.x * r_b.y);
    k[1].x = k[0].y;
    k[1].y = (bdata_a.inv_mass + bdata_b.inv_mass) +
             (bdata_a.inv_mmoi * math::square(r_a.x)) +
             (bdata_b.inv_mmoi * math::square(r_b.x));

    auto impulse = -k.solve(p);

    bdata_a.pos -= bdata_a.inv_mass * impulse;
    bdata_a.angle -= bdata_a.inv_mmoi * math::perp_dot(r_a, impulse);
    bdata_b.pos += bdata_b.inv_mass * impulse;
    bdata_b.angle += bdata_b.inv_mmoi * math::perp_dot(r_b, impulse);

	return (linear_error <= LINEAR_SLOP) && (angular_error <= ANGULAR_SLOP);
}

} // namespace physics
} // namespace rdge
