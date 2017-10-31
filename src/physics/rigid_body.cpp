#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>
#include <rdge/util/logger.hpp>

namespace rdge {
namespace physics {

using namespace rdge::math;

RigidBody::RigidBody (const rigid_body_profile& prof, CollisionGraph* parent)
    : graph(parent)
    , user_data(prof.user_data)
    , world_transform(prof.position, prof.angle)
    , gravity_scale(prof.gravity_scale)
    , m_type(prof.type)
{
    linear.velocity = prof.linear_velocity;
    linear.damping = prof.linear_damping;
    angular.velocity = prof.angular_velocity;
    angular.damping = prof.angular_damping;

    if (prof.simulate)
    {
        m_flags |= SIMULATE;
    }

    if (prof.awake)
    {
        m_flags |= AWAKE;
    }

    if (prof.prevent_rotation)
    {
        m_flags |= PREVENT_ROTATION;
    }

    if (prof.prevent_sleep)
    {
        m_flags |= PREVENT_SLEEP;
    }

    sweep.pos_0 = world_transform.pos;
    sweep.pos_n = world_transform.pos;
    sweep.angle_0 = prof.angle;
    sweep.angle_n = prof.angle;

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
        result->proxy->handle = graph->RegisterProxy(result->proxy);
    }

    if (result->density > 0.f)
    {
        ComputeMass();
    }

    return result;
}

Fixture*
RigidBody::CreateFixture (ishape* shape, float density)
{
    fixture_profile p;
    p.shape = shape;
    p.density = density;

    return CreateFixture(p);
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

void
RigidBody::Enable (void)
{
    SDL_assert(!graph->IsLocked());

    if (!IsSimulating())
    {
        m_flags |= SIMULATE;

        fixtures.for_each([=](auto* f) {
            f->proxy->handle = graph->RegisterProxy(f->proxy);
        });
    }
}

void
RigidBody::Disable (void)
{
    SDL_assert(!graph->IsLocked());

    if (IsSimulating())
    {
        m_flags &= ~SIMULATE;

        fixtures.for_each([=](auto* f) {
            graph->UnregisterProxy(f->proxy);
            f->proxy->handle = fixture_proxy::INVALID_HANDLE;
        });

        contact_edges.for_each([=](auto* edge) {
            graph->DestroyContact(edge->contact);
        });
    }
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
RigidBody::SetPosition (math::vec2 pos)
{
    world_transform.pos = pos;
    sweep.pos_n = world_transform.to_world(sweep.local_center);
    sweep.pos_0 = sweep.pos_n;

    SyncFixtures();
}

void
RigidBody::SyncFixtures (void)
{
    // world transform has been updated.  the fixtures need to reset their
    // world shape and proxies (set to the swept shape over the time step)

    iso_transform sweep_start;
    sweep_start.set_angle(sweep.angle_0);
    sweep_start.pos = sweep.pos_0 - sweep_start.rot.rotate(sweep.local_center);

    auto displacement = world_transform.pos - sweep_start.pos;
    fixtures.for_each([&](auto* f) {
        f->Syncronize();

        aabb box_0 = f->shape.local->compute_aabb(sweep_start);
        aabb box_n = f->shape.local->compute_aabb(world_transform);

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
        // Adjust mmoi to the bodies center of mass
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
    sweep.pos_n = world_transform.to_world(sweep.local_center);
    sweep.pos_0 = sweep.pos_n;

    // Update velocity to the new center of mass
    linear.velocity += (sweep.pos_n - old_center).perp() * angular.velocity;
}

std::ostream& operator<< (std::ostream& os, RigidBodyType value)
{
    switch (value)
    {
    case RigidBodyType::STATIC:
        return os << "STATIC";
    case RigidBodyType::KINEMATIC:
        return os << "KINEMATIC";
    case RigidBodyType::DYNAMIC:
        return os << "DYNAMIC";
    default:
        break;
    }

    return os << "UNKNOWN";
}

std::ostream& operator<< (std::ostream& os, const RigidBody& b)
{
    os << "RigidBody: {"
       << "\n  type=" << b.GetType()
       << "\n  gravity_scale=" << b.gravity_scale
       << "\n  flags:"
       << "\n    simulating=" << std::boolalpha << b.IsSimulating()
       << "\n    awake=" << std::boolalpha << b.IsAwake()
       << "\n    sleep_prevented=" << std::boolalpha << b.IsSleepPrevented()
       << "\n    fixed_rotation=" << std::boolalpha << b.IsFixedRotation()
       << "\n  collections:"
       << "\n    fixtures=" << b.fixtures.count
       << "\n    contacts=" << b.contact_edges.count
       << "\n    joints=" << b.joint_edges.count
       << "\n  sweep:"
       << "\n    local_center=" << b.sweep.local_center
       << "\n    pos_0=" << b.sweep.pos_0
       << "\n    pos_n=" << b.sweep.pos_n
       << "\n    angle_0=" << b.sweep.angle_0
       << "\n    angle_n=" << b.sweep.angle_n
       << "\n  linear_motion:"
       << "\n    velocity=" << b.linear.velocity
       << "\n    force=" << b.linear.force
       << "\n    damping=" << b.linear.damping
       << "\n    mass=" << b.linear.mass
       << "\n    inv_mass=" << b.linear.inv_mass
       << "\n  angular_motion:"
       << "\n    velocity=" << b.angular.velocity
       << "\n    torque=" << b.angular.torque
       << "\n    damping=" << b.angular.damping
       << "\n    mmoi=" << b.angular.mmoi
       << "\n    inv_mmoi=" << b.angular.inv_mmoi
       << "\n}\n";

    return os;
}

} // namespace physics
} // namespace rdge
