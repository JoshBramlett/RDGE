#include <rdge/physics/collision_graph.hpp>

#include <algorithm> // remove_if

namespace rdge {
namespace physics {

namespace {

ContactFilter s_defaultContactFilter;
GraphListener s_defaultGraphListener;

struct island_solver
{
    struct internal_pos
    {
        math::vec2 center;
        float      angle;
    };

    struct internal_vel
    {
        math::vec2 linear;
        float      angular;
    };

    std::vector<RigidBody*> bodies;
    std::vector<Contact*> contacts;
    std::vector<internal_pos> positions;
    std::vector<internal_vel> velocities;

    math::vec2 gravity;
    float dt;

    void init (float delta_time, size_t body_count, size_t contact_count)
    {
        dt = delta_time;

        bodies.reserve(body_count);
        positions.reserve(body_count);
        velocities.reserve(body_count);

        contacts.reserve(contact_count);
    }

    void add (RigidBody* b)
    {
        b->island_index = bodies.size();
        bodies.push_back(b);

        // Store positions for continuous collision
        b->sweep.angle_0 = b->sweep.angle_n;
        b->sweep.pos_0 = b->sweep.pos_n;

        internal_pos p { b->sweep.pos_n, b->sweep.angle_n };
        internal_vel v { b->linear.velocity, b->angular.velocity };

        if (b->GetType() == RigidBodyType::DYNAMIC)
        {
            auto linear_acc = (b->gravity_scale * gravity) +
                              (b->linear.force * b->linear.inv_mass);
            auto angular_acc = (b->angular.torque * b->angular.inv_mmoi);

            // integrate velocities
            v.linear += linear_acc * dt;
            v.angular += angular_acc * dt;

            // apply damping
            //
            // From Box2D:
            // ODE: dv/dt + c * v = 0
            // Solution: v(t) = v0 * exp(-c * t)
            // Time step: v(t + dt)
            //            = v0 * exp(-c * (t + dt))
            //            = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
            // v2 = exp(-c * dt) * v1
            // Pade approximation:
            // v2 = v1 * 1 / (1 + c * dt)
            v.linear *= 1.f / (1.f + dt * b->linear.damping);
            v.angular *= 1.f / (1.f + dt * b->angular.damping);
        }

        positions.push_back(p);
        velocities.push_back(v);
    }

    void add (Contact* c)
    {
        contacts.push_back(c);
    }

    void clear (void)
    {
        bodies.clear();
        contacts.clear();
        positions.clear();
        velocities.clear();
    }

    void solve (void)
    {
        for (auto contact : contacts)
        {
            auto& mf = contact->manifold;
            auto body_a = contact->fixture_a->body;
            auto body_b = contact->fixture_b->body;
            for (size_t i = 0; i < mf.count; i++)
            {
                auto& vel_a = velocities[body_a->island_index].linear;
                auto& vel_b = velocities[body_b->island_index].linear;

                float d = math::dot((vel_b - vel_a), mf.normal);

                // impulse scalar
                float j = -(1.f + contact->restitution) * d;
                j *= body_a->linear.inv_mass + body_b->linear.inv_mass;

                auto impulse = mf.normal * j;
                vel_a -= impulse * body_a->linear.inv_mass;
                vel_b += impulse * body_b->linear.inv_mass;
            }
        }

        // Copy state buffers back to the bodies
        for (size_t i = 0; i < bodies.size(); i++)
        {
            auto& p = positions[i];
            auto& v = velocities[i];

            auto body = bodies[i];
            body->sweep.pos_n = p.center + (v.linear * dt);
            body->sweep.angle_n = p.angle + (v.angular * dt);
            body->linear.velocity = v.linear;
            body->angular.velocity = v.angular;

            body->world_transform.set_angle(body->sweep.angle_n);
            body->world_transform.pos = body->sweep.pos_n -
                                        body->world_transform.rot.rotate(body->sweep.local_center);
        }
    }

} s_island;

} // anonymous namespace

CollisionGraph::CollisionGraph (const math::vec2& g)
    : gravity(g)
    , custom_filter(&s_defaultContactFilter)
    , listener(&s_defaultGraphListener)
{
    m_dirtyProxies.reserve(128);

    // TODO this will be a problem if there are multiple instances
    s_island.gravity = g;
}

CollisionGraph::~CollisionGraph (void) noexcept
{
    bodies.for_each([=](auto* b) {
        block_allocator.Delete<RigidBody>(b);
    });

    contacts.for_each([=](auto* c) {
        block_allocator.Delete<Contact>(c);
    });
}

RigidBody*
CollisionGraph::CreateBody (const rigid_body_profile& profile)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return nullptr;
    }

    RigidBody* result = block_allocator.New<RigidBody>(profile, this);
    bodies.push_back(result);

    return result;
}

void
CollisionGraph::DestroyBody (RigidBody* body)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return;
    }

    body->fixtures.for_each([=](auto* f) {
        body->DestroyFixture(f);
    });

    body->contact_edges.for_each([=](auto* edge) {
        DestroyContact(edge->contact);
    });

    bodies.remove(body);
    block_allocator.Delete<RigidBody>(body);
}

void
CollisionGraph::Step (float dt)
{
    m_flags |= LOCKED;

    // 1) update contact list
    {
        // find new contacts for any added proxies
        if (!m_dirtyProxies.empty())
        {
            auto pairs = m_tree.Query<fixture_proxy>(m_dirtyProxies);
            for (auto& p : pairs)
            {
                CreateContact(p.first, p.second);
            }

            m_dirtyProxies.clear();
        }

        // remove all contacts that are not colliding
        PurgeContacts();
    }

    // 2) integration and contact solving
    {
        // clear island flags

        // TODO we iterate through the body list three times in this step.  Clean this up.
        bodies.for_each([](auto* body) {
            body->m_flags &= ~RigidBody::ON_ISLAND;
        });

        contacts.for_each([](auto* contact) {
            contact->m_flags &= ~Contact::ON_ISLAND;
        });

        static std::vector<RigidBody*> body_stack;
        body_stack.reserve(bodies.count);

        s_island.init(dt, bodies.count, contacts.count);
        bodies.for_each([&](auto* body) {
            if ((body->m_flags & RigidBody::ON_ISLAND) ||
                !body->IsSimulating() ||
                !body->IsAwake())
            {
                return;
            }

            body_stack.clear();
            s_island.clear();

            body_stack.push_back(body);
            while (!body_stack.empty())
            {
                RigidBody* b = body_stack.back();
                body_stack.pop_back();

                s_island.add(b);
                body->m_flags |= RigidBody::ON_ISLAND;

                // Box2D: Make sure the body is awake (without resetting sleep timer).
                // TODO ??? Why?
                body->m_flags |= RigidBody::AWAKE;

                // to keep islands small do not propogate past static bodies
                if (b->m_type == RigidBodyType::STATIC)
                {
                    continue;
                }

                b->contact_edges.for_each([&](auto* edge) {
                    Contact* c = edge->contact;

                    // TODO could be simplified to m_flags != 0 (except sensor test),
                    //      but for future proofing should remain as is.  Look into
                    //      IsTouching to see where it's used.
                    if ((c->m_flags & Contact::ON_ISLAND) ||
                        !c->IsTouching() ||
                        !c->IsEnabled() ||
                        (c->fixture_a->IsSensor() || c->fixture_b->IsSensor()))
                    {
                        return;
                    }

                    c->m_flags |= Contact::ON_ISLAND;
                    s_island.contacts.push_back(c);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        body_stack.push_back(edge->other);
                    }
                });
            }

            s_island.solve();

            // TODO Box2D iterates through the island and removes the island from
            //      all static bodies.  I think this step can be removed simply
            //      by having an OnIsland getter return false if static
        });

        bodies.for_each([](auto* body) {
            // If a body was not in an island then it did not move.
            if (((body->m_flags & RigidBody::ON_ISLAND) == 0) ||
                (body->m_type == RigidBodyType::STATIC))
            {
                return;
            }

            body->SyncFixtures();
        });
    }

    m_flags &= ~LOCKED;
}

void
CollisionGraph::CreateContact (fixture_proxy* a, fixture_proxy* b)
{
    RigidBody* body_a = a->fixture->body;
    RigidBody* body_b = b->fixture->body;

    if (!body_a->ShouldCollide(body_b))
    {
        return;
    }

    if (custom_filter && !custom_filter->ShouldCollide(a->fixture, b->fixture))
    {
        return;
    }

    if (body_a->HasEdge(a->fixture, b->fixture))
    {
        return;
    }

    Contact* contact = block_allocator.New<Contact>(a->fixture, b->fixture);
    contacts.push_back(contact);
    body_a->contact_edges.push_back(&contact->edge_a);
    body_b->contact_edges.push_back(&contact->edge_b);

    if (!contact->fixture_a->IsSensor() &&
        !contact->fixture_b->IsSensor())
    {
        body_a->WakeUp();
        body_b->WakeUp();
    }

    std::cout << "Contact created" << std::endl;
}

void
CollisionGraph::DestroyContact (Contact* contact)
{
    RigidBody* body_a = contact->fixture_a->body;
    RigidBody* body_b = contact->fixture_b->body;

    if (contact->IsTouching())
    {
        if (listener)
        {
            listener->OnContactEnd(contact);
        }

        if (!contact->fixture_a->IsSensor() &&
            !contact->fixture_b->IsSensor())
        {
            body_a->WakeUp();
            body_b->WakeUp();
        }
    }

    contacts.remove(contact);
    body_a->contact_edges.remove(&contact->edge_a);
    body_b->contact_edges.remove(&contact->edge_b);

    block_allocator.Delete<Contact>(contact);

    std::cout << "Contact destroyed" << std::endl;
}

void
CollisionGraph::PurgeContacts (void)
{
    contacts.for_each([this](auto* contact) {
        Fixture* a = contact->fixture_a;
        Fixture* b = contact->fixture_b;

        if (a->IsFilterDirty() || b->IsFilterDirty())
        {
            if (a->body->ShouldCollide(b->body) == false)
            {
                DestroyContact(contact);
                return;
            }

            if (custom_filter && custom_filter->ShouldCollide(a, b) == false)
            {
                DestroyContact(contact);
                return;
            }

            a->FlagFilterClean();
            b->FlagFilterClean();
        }

        // skip intersecion tests for sleeping bodies
        if (!a->body->IsAwake() && !b->body->IsAwake())
        {
            return;
        }

        // purge non-intersecting contacts.  check is on the enlarged AABBs
        if (!m_tree.Intersects(a->proxy->handle, b->proxy->handle))
        {
            DestroyContact(contact);
            return;
        }

        contact->Update(listener);
    });
}

int32
CollisionGraph::RegisterProxy (fixture_proxy* proxy)
{
    int32 handle = m_tree.CreateProxy(proxy->box, proxy);
    m_dirtyProxies.push_back(handle);

    std::cout << "register: " << m_tree.Dump() << std::endl;

    return handle;
}

void
CollisionGraph::UnregisterProxy (const fixture_proxy* proxy)
{
    int32 handle = proxy->handle;
    if (handle == fixture_proxy::INVALID_HANDLE)
    {
        return;
    }

    m_tree.DestroyProxy(handle);
    m_dirtyProxies.erase(std::remove_if(m_dirtyProxies.begin(),
                                        m_dirtyProxies.end(),
                                        [=](int32 h) { return h == handle; }));

    std::cout << "unregister: " << m_tree.Dump() << std::endl;
}

void
CollisionGraph::MoveProxy (const fixture_proxy* proxy, const math::vec2& displacement)
{
    m_tree.MoveProxy(proxy->handle, proxy->box, displacement);
    m_dirtyProxies.push_back(proxy->handle);
}

void
CollisionGraph::TouchProxy (const fixture_proxy* proxy)
{
    m_dirtyProxies.push_back(proxy->handle);
}

} // namespace physics
} // namespace rdge
