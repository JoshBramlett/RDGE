#include <rdge/physics/solver.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/contact.hpp>
#include <rdge/physics/collision.hpp>
#include <rdge/physics/collision_graph.hpp>

#include <limits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

namespace {

// squared to avoid sqrt
constexpr float MAX_TRANSLATION_SQAURED = math::square(Solver::MAX_TRANSLATION);
constexpr float MAX_ROTATION_SQUARED = math::square(Solver::MAX_ROTATION);
constexpr float LINEAR_SLEEP_TOLERANCE_SQUARED = math::square(Solver::LINEAR_SLEEP_TOLERANCE);
constexpr float ANGULAR_SLEEP_TOLERANCE_SQUARED = math::square(Solver::ANGULAR_SLEEP_TOLERANCE);

} // anonymous namespace

void
Solver::Initialize (float delta_time, size_t body_count, size_t contact_count)
{
    m_dt = delta_time;
    m_bodies.reserve(body_count);
    m_contacts.reserve(contact_count);
}

void
Solver::Add (RigidBody* b)
{
    // Store positions for continuous collision
    b->sweep.angle_0 = b->sweep.angle_n;
    b->sweep.pos_0 = b->sweep.pos_n;

    // Awake flag is set b/c for a body to be added to the island it
    // was already awake or now in contact with an awake body
    b->m_flags |= RigidBody::ON_ISLAND | RigidBody::AWAKE;
    b->solver_index = m_bodies.size();

    auto& data = m_bodies.next();
    data.body = b;
    data.pos = b->sweep.pos_n;
    data.linear_vel = b->linear.velocity;
    data.angle = b->sweep.angle_n;
    data.angular_vel = b->angular.velocity;
    data.inv_mass = b->linear.inv_mass;
    data.inv_mmoi = b->angular.inv_mmoi;

    if (b->GetType() == RigidBodyType::DYNAMIC)
    {
        // Perform initial velocity integration and apply damping
        auto linear_acc = (b->gravity_scale * gravity) +
                          (b->linear.force * b->linear.inv_mass);
        auto angular_acc = (b->angular.torque * b->angular.inv_mmoi);

        data.linear_vel += linear_acc * m_dt;
        data.angular_vel += angular_acc * m_dt;

        // From Box2D regarding damping:
        // ODE: dv/dt + c * v = 0
        // Solution: v(t) = v0 * exp(-c * t)
        // Time step: v(t + dt)
        //            = v0 * exp(-c * (t + dt))
        //            = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
        // v2 = exp(-c * dt) * v1
        // Pade approximation:
        // v2 = v1 * 1 / (1 + c * dt)
        data.linear_vel *= 1.f / (1.f + m_dt * b->linear.damping);
        data.angular_vel *= 1.f / (1.f + m_dt * b->angular.damping);
    }
}

void
Solver::Add (Contact* c)
{
    c->m_flags |= Contact::ON_ISLAND;
    auto& data = m_contacts.next();
    data.contact = c;
}

void
Solver::Clear (void)
{
    m_bodies.clear();
    m_contacts.clear();
}

void
Solver::Solve (void)
{
    for (auto& data : m_contacts)
    {
        // populate relevant body data
        const auto body_a = data.contact->fixture_a->body;
        const auto body_b = data.contact->fixture_b->body;

        data.body_index[0] = body_a->solver_index;
        data.body_index[1] = body_b->solver_index;

        auto& bdata_a = m_bodies[data.body_index[0]];
        auto& bdata_b = m_bodies[data.body_index[1]];
        data.combined_inv_mass = bdata_a.inv_mass + bdata_b.inv_mass;

        // build the velocity constraint points
        const auto& mf = data.contact->manifold;
        math::vec2 tangent = mf.normal.perp_ccw();
        float restitution = data.contact->restitution;

        for (size_t i = 0; i < mf.count; i++)
        {
            auto& vcp = data.points[i];

            vcp.rel_point[0] = mf.contacts[i] - bdata_a.pos;
            vcp.rel_point[1] = mf.contacts[i] - bdata_b.pos;

            // effective mass on the normal
            float radius_normal_a = math::perp_dot(vcp.rel_point[0], mf.normal);
            float radius_normal_b = math::perp_dot(vcp.rel_point[1], mf.normal);
            float enm = data.combined_inv_mass +
                        (bdata_a.inv_mmoi * math::square(radius_normal_a)) +
                        (bdata_b.inv_mmoi * math::square(radius_normal_b));
            vcp.normal_mass = (enm > 0.f) ? (1.f / enm) : 0.f;

            // effective mass on the tangent
            float radius_tangent_a = math::perp_dot(vcp.rel_point[0], tangent);
            float radius_tangent_b = math::perp_dot(vcp.rel_point[1], tangent);
            float etm = data.combined_inv_mass +
                        (bdata_a.inv_mmoi * math::square(radius_tangent_a)) +
                        (bdata_b.inv_mmoi * math::square(radius_tangent_b));
            vcp.tangent_mass = (etm > 0.f) ? (1.f / etm) : 0.f;

            // relative velocity along direction of contact normal
            math::vec2 vel_a = bdata_a.linear_vel +
                               (vcp.rel_point[0].perp() * bdata_a.angular_vel);
            math::vec2 vel_b = bdata_b.linear_vel +
                               (vcp.rel_point[1].perp() * bdata_b.angular_vel);
            float rnv = math::dot(mf.normal, vel_b - vel_a);

            vcp.velocity_bias = 0.f;
            if (rnv < -VELOCITY_THRESHOLD)
            {
                vcp.velocity_bias = rnv * -restitution;
            }
        }
    }

    for (size_t iter = 0; iter < velocity_iterations; iter++)
    {
        SolveVelocityConstraints();
    }

    for (auto& data : m_bodies)
    {
        auto t = data.linear_vel * m_dt;
        if (t.self_dot() > MAX_TRANSLATION_SQAURED)
        {
            data.linear_vel *= MAX_TRANSLATION / t.length();
        }

        auto r = data.angular_vel * m_dt;
        if (math::square(r) > MAX_ROTATION_SQUARED)
        {
            data.angular_vel *= MAX_ROTATION / math::abs(r);
        }

        data.pos += data.linear_vel * m_dt;
        data.angle += data.angular_vel * m_dt;
    }

    m_positionsSolved = false;
    for (size_t iter = 0; iter < position_iterations; iter++)
    {
        m_positionsSolved = CorrectPositions();
        if (m_positionsSolved)
        {
            break;
        }
    }

    for (auto& data : m_bodies)
    {
        auto body = data.body;
        body->sweep.pos_n = data.pos;
        body->sweep.angle_n = data.angle;
        body->linear.velocity = data.linear_vel;
        body->angular.velocity = data.angular_vel;

        auto& xf = body->world_transform;
        xf.set_angle(body->sweep.angle_n);
        xf.pos = body->sweep.pos_n - xf.rot.rotate(body->sweep.local_center);

        // allow static bodies to be added to other islands
        if (body->m_type == RigidBodyType::STATIC)
        {
            body->m_flags &= ~RigidBody::ON_ISLAND;
        }
    }
}

void
Solver::ProcessPostSolve (const CollisionGraph& graph)
{
    if (graph.listener)
    {
        for (auto& data : m_contacts)
        {
            graph.listener->OnPostSolve(data.contact);
        }
    }

    if (!graph.IsSleepPrevented() && m_positionsSolved)
    {
        float min_sleep_time = std::numeric_limits<float>::max();
        for (auto& data : m_bodies)
        {
            auto body = data.body;
            if (body->m_type == RigidBodyType::STATIC)
            {
                continue;
            }

            if (body->IsSleepPrevented() ||
                body->linear.velocity.self_dot() > LINEAR_SLEEP_TOLERANCE_SQUARED ||
                math::square(body->angular.velocity) > ANGULAR_SLEEP_TOLERANCE_SQUARED)
            {
                body->m_sleepTime = 0.f;
                min_sleep_time = 0.f;
            }
            else
            {
                body->m_sleepTime += m_dt;
                min_sleep_time = std::min(min_sleep_time, body->m_sleepTime);
            }
        }

        if (min_sleep_time >= SLEEP_THRESHOLD)
        {
            for (auto& data : m_bodies)
            {
                data.body->Sleep();
            }
        }
    }
}

void
Solver::SolveVelocityConstraints (void)
{
    for (auto& data : m_contacts)
    {
        auto& bdata_a = m_bodies[data.body_index[0]];
        auto& bdata_b = m_bodies[data.body_index[1]];

        const auto& mf = data.contact->manifold;
        math::vec2 tangent = mf.normal.perp_ccw();
        float tangent_speed = data.contact->tangent_speed;
        float friction = data.contact->friction;

        auto& impulse_cache = data.contact->impulse;
        impulse_cache.count = mf.count;

        for (size_t i = 0; i < mf.count; i++)
        {
            // Solve tangent constraints first b/c non-penetration is more
            // important than friction

            auto& vcp = data.points[i];

            // relative velocity along direction of contact tangent
            math::vec2 vel_a = bdata_a.linear_vel +
                               (vcp.rel_point[0].perp() * bdata_a.angular_vel);
            math::vec2 vel_b = bdata_b.linear_vel +
                               (vcp.rel_point[1].perp() * bdata_b.angular_vel);
            float rtv = math::dot(tangent, vel_b - vel_a) - tangent_speed;

            // Compute tangent force
            float lambda = vcp.tangent_mass * (-rtv);

            // clamp the accumulated force
            float max_friction = friction * vcp.normal_impulse;
            float new_impulse = math::clamp(vcp.tangent_impulse + lambda,
                                            -max_friction, max_friction);
            lambda = new_impulse - vcp.tangent_impulse;
            vcp.tangent_impulse = new_impulse;

            // cache impulses (for OnPostSolve)
            impulse_cache.tangents[i] = vcp.tangent_impulse;

            // apply contact impulse
            math::vec2 impulse = tangent * lambda;

            bdata_a.linear_vel -= bdata_a.inv_mass * impulse;
            bdata_a.angular_vel -= bdata_a.inv_mmoi *
                                   math::perp_dot(vcp.rel_point[0], impulse);

            bdata_b.linear_vel += bdata_b.inv_mass * impulse;
            bdata_b.angular_vel += bdata_b.inv_mmoi *
                                   math::perp_dot(vcp.rel_point[1], impulse);
        }

        for (size_t i = 0; i < mf.count; i++)
        {
            // Solve normal constraints

            auto& vcp = data.points[i];

            // relative velocity along direction of contact normal
            math::vec2 vel_a = bdata_a.linear_vel +
                               (vcp.rel_point[0].perp() * bdata_a.angular_vel);
            math::vec2 vel_b = bdata_b.linear_vel +
                               (vcp.rel_point[1].perp() * bdata_b.angular_vel);
            float rnv = math::dot(mf.normal, vel_b - vel_a);

            // Compute normal force
            float lambda = -vcp.normal_mass * (rnv - vcp.velocity_bias);

            // clamp the accumulated force
            float new_impulse = std::max(vcp.normal_impulse + lambda, 0.f);
            lambda = new_impulse - vcp.normal_impulse;
            vcp.normal_impulse = new_impulse;

            // cache impulses (for OnPostSolve)
            impulse_cache.normals[i] = vcp.normal_impulse;

            // apply contact impulse
            math::vec2 impulse = mf.normal * lambda;

            bdata_a.linear_vel -= bdata_a.inv_mass * impulse;
            bdata_a.angular_vel -= bdata_a.inv_mmoi *
                                   math::perp_dot(vcp.rel_point[0], impulse);

            bdata_b.linear_vel += bdata_b.inv_mass * impulse;
            bdata_b.angular_vel += bdata_b.inv_mmoi *
                                   math::perp_dot(vcp.rel_point[1], impulse);
        }
    }
}

bool
Solver::CorrectPositions (void)
{
    float max_depth = 0.f;

    for (auto& data : m_contacts)
    {
        auto& bdata_a = m_bodies[data.body_index[0]];
        auto& bdata_b = m_bodies[data.body_index[1]];

        const auto& mf = data.contact->manifold;
        math::vec2 normal = mf.flip_dominant ? -mf.normal : mf.normal;

        for (size_t i = 0; i < mf.count; i++)
        {
            // For math involved: http://bit.ly/2ul0uEw
            auto& vcp = data.points[i];

            if (mf.depths[i] > max_depth)
            {
                max_depth = mf.depths[i];
            }

            float C = math::clamp(SCALE_FACTOR * (-mf.depths[i] + LINEAR_SLOP),
                                  -MAX_LINEAR_CORRECTION, 0.f);

            float radius_normal_a = math::perp_dot(vcp.rel_point[0], normal);
            float radius_normal_b = math::perp_dot(vcp.rel_point[1], normal);

            // effective mass on the normal
            float enm = data.combined_inv_mass +
                        (bdata_a.inv_mmoi * math::square(radius_normal_a)) +
                        (bdata_b.inv_mmoi * math::square(radius_normal_b));
            float normal_mass = (enm > 0.f) ? (-C / enm) : 0.f;
            auto impulse = normal * normal_mass;

            bdata_a.pos -= bdata_a.inv_mass * impulse;
            bdata_a.angle -= bdata_a.inv_mmoi *
                             math::perp_dot(vcp.rel_point[0], impulse);

            bdata_b.pos += bdata_b.inv_mass * impulse;
            bdata_b.angle += bdata_b.inv_mmoi *
                             math::perp_dot(vcp.rel_point[1], impulse);
        }
    }

    return max_depth <= LINEAR_SLOP * 3.f;
}

} // namespace physics
} // namespace rdge
