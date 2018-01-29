#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/joints/revolute_joint.hpp>
#include <rdge/util/profiling.hpp>

#include <algorithm> // remove_if

namespace rdge {
namespace physics {

namespace {

ContactFilter s_defaultContactFilter;
GraphListener s_defaultGraphListener;

} // anonymous namespace

CollisionGraph::CollisionGraph (const math::vec2& g)
    : custom_filter(&s_defaultContactFilter)
    , listener(&s_defaultGraphListener)
    , m_solver(&m_step)
    , m_flags(CLEAR_FORCES)
{
    m_dirtyProxies.reserve(128);
    m_solver.gravity = g;
}

CollisionGraph::~CollisionGraph (void) noexcept
{
    ClearGraph();
}

void
CollisionGraph::ClearGraph (void) noexcept
{
    m_bodies.for_each([=](auto* b) {
        DestroyBody(b);
    });

    m_dirtyProxies.clear();
    m_tree.ClearProxies();
    block_allocator.Clear();

    SDL_assert(m_bodies.size() == 0);
    SDL_assert(m_contacts.size() == 0);
    SDL_assert(m_joints.size() == 0);
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
    m_bodies.push_back(*result);

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

    body->joint_edges.for_each([=](auto* edge) {
        DestroyJoint(edge->joint);
    });

    // NOTE Contacts destroyed by DestroyFixture
    body->fixtures.for_each([=](auto* f) {
        body->DestroyFixture(f);
    });

    m_bodies.remove(*body);
    block_allocator.Delete<RigidBody>(body);
}

RevoluteJoint*
CollisionGraph::CreateRevoluteJoint (RigidBody* a, RigidBody* b, math::vec2 anchor)
{
    if (IsLocked())
    {
        SDL_assert(false);
        return nullptr;
    }

    RevoluteJoint* result = block_allocator.New<RevoluteJoint>(a, b, anchor);
    m_joints.push_back(*result);
    a->joint_edges.push_back(result->edge_a);
    b->joint_edges.push_back(result->edge_b);

    return result;
}

void
CollisionGraph::DestroyJoint (BaseJoint* joint)
{
    if (IsLocked())
    {
        SDL_assert(false);
    }

    RigidBody* body_a = joint->body_a;
    RigidBody* body_b = joint->body_b;

    body_a->WakeUp();
    body_b->WakeUp();

    // If the joint was preventing collisions, flag fixtures for filtering
    if (!joint->ShouldCollide())
    {
        body_a->contact_edges.for_each([=](auto* edge) {
            if (edge->other == body_b)
            {
                edge->contact->fixture_a->FlagFilterDirty();
                edge->contact->fixture_b->FlagFilterDirty();
            }
        });
    }

    m_joints.remove(*joint);
    body_a->joint_edges.remove(joint->edge_a);
    body_b->joint_edges.remove(joint->edge_b);

    // TODO update once more joints are added
    block_allocator.Delete<RevoluteJoint>(static_cast<RevoluteJoint*>(joint));
}

void
CollisionGraph::Step (float dt)
{
    m_flags |= LOCKED;

    SDL_assert(dt > 0.f);
    m_step.dt = dt;
    m_step.inv = 1.f / dt;
    m_step.ratio = m_step.inv_0 * dt;

    // 1) update contact list
    {
        // find new contacts for any added proxies
        if (!m_dirtyProxies.empty())
        {
            ScopeProfiler<> p(&debug_profile.create_contacts);
            auto pairs = m_tree.Query<fixture_proxy>(m_dirtyProxies);
            for (auto& p : pairs)
            {
                CreateContact(p.first, p.second);
            }

            m_dirtyProxies.clear();
        }

        // remove all contacts that are not colliding
        ScopeProfiler<> p(&debug_profile.purge_contacts);
        PurgeContacts();
    }

    {
        // NOTE: Island flags must be reset prior to solving.  To avoid another
        //       iteration to reset the flags, they are performed at:
        //   - bodies reset at the end of the step
        //   - contacts reset during contact purging
        //   - joints done here

        // TODO Remove this when physics engine gets more mileage.

#ifdef RDGE_DEBUG
        m_bodies.for_each([](auto* body) {
            SDL_assert((body->m_flags & RigidBody::ON_ISLAND) == 0);
        });

        m_contacts.for_each([](auto* contact) {
            SDL_assert((contact->m_flags & Contact::ON_ISLAND) == 0);
        });
#endif

        m_joints.for_each([](auto* joint) {
            joint->m_flags &= ~BaseJoint::ON_ISLAND;
        });
    }

    // 2) integration and contact solving
    {
        ScopeProfiler<> p(&debug_profile.solve);
        static std::vector<RigidBody*> body_stack;
        body_stack.reserve(m_bodies.size());

        m_solver.Initialize(m_bodies.size(), m_contacts.size(), m_joints.size());
        m_bodies.for_each([&](auto* body) {
            if ((body->m_flags & RigidBody::ON_ISLAND) ||
                !body->IsSimulating() ||
                !body->IsAwake())
            {
                return;
            }

            body_stack.clear();
            m_solver.Clear();

            body->m_flags |= RigidBody::ON_ISLAND;
            body_stack.push_back(body);
            while (!body_stack.empty())
            {
                RigidBody* b = body_stack.back();
                body_stack.pop_back();

                m_solver.Add(b);

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
                    m_solver.Add(c);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        edge->other->m_flags |= RigidBody::ON_ISLAND;
                        body_stack.push_back(edge->other);
                    }
                });

                b->joint_edges.for_each([&](auto* edge) {
                    BaseJoint* j = edge->joint;

                    // TODO could be simplified to m_flags != 0 (except sensor test),
                    //      but for future proofing should remain as is.  Look into
                    //      IsTouching to see where it's used.
                    if (j->m_flags & BaseJoint::ON_ISLAND)
                    {
                        return;
                    }

                    if (!j->body_a->IsSimulating() || !j->body_b->IsSimulating())
                    {
                        return;
                    }

                    j->m_flags |= BaseJoint::ON_ISLAND;
                    m_solver.Add(j);

                    if ((edge->other->m_flags & RigidBody::ON_ISLAND) == 0)
                    {
                        edge->other->m_flags |= RigidBody::ON_ISLAND;
                        body_stack.push_back(edge->other);
                    }
                });
            }

            m_solver.Solve();
            m_solver.ProcessPostSolve(*this);
        });
    }

    {
        ScopeProfiler<> p(&debug_profile.synchronize);
        m_bodies.for_each([=](auto* body) {
            // If a body was not in an island then it did not move.
            if (body->m_flags & RigidBody::ON_ISLAND)
            {
                if (body->m_type != RigidBodyType::STATIC)
                {
                    body->SyncFixtures();

                    if (m_flags & CLEAR_FORCES)
                    {
                        body->linear.force = { 0.f, 0.f };
                        body->angular.torque = 0.f;
                    }
                }

                // Remove flag for the next iteration
                body->m_flags &= ~RigidBody::ON_ISLAND;
            }
        });
    }

    m_step.dt_0 = m_step.dt;
    m_step.inv_0 = m_step.inv;

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
    m_contacts.push_back(*contact);
    body_a->contact_edges.push_back(contact->edge_a);
    body_b->contact_edges.push_back(contact->edge_b);

    if (!contact->fixture_a->IsSensor() &&
        !contact->fixture_b->IsSensor())
    {
        body_a->WakeUp();
        body_b->WakeUp();
    }
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

    m_contacts.remove(*contact);
    body_a->contact_edges.remove(contact->edge_a);
    body_b->contact_edges.remove(contact->edge_b);

    block_allocator.Delete<Contact>(contact);
}

void
CollisionGraph::PurgeContacts (void)
{
    m_contacts.for_each([this](auto* contact) {
        contact->m_flags &= ~Contact::ON_ISLAND;

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
                                        [=](int32 h) { return h == handle; }),
                                        m_dirtyProxies.end());
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
