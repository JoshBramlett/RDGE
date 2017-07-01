#include <rdge/physics/collision_graph.hpp>

#ifdef RDGE_DEBUG
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>
#endif

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
    , m_flags(CLEAR_FORCES)
{
    m_dirtyProxies.reserve(128);
    m_solver.gravity = g;
}

CollisionGraph::~CollisionGraph (void) noexcept
{
    m_bodies.for_each([=](auto* b) {
        block_allocator.Delete<RigidBody>(b);
    });

    m_contacts.for_each([=](auto* c) {
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
    m_bodies.push_back(result);

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

    m_bodies.remove(body);
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
        m_bodies.for_each([](auto* body) {
            body->m_flags &= ~RigidBody::ON_ISLAND;
        });

        m_contacts.for_each([](auto* contact) {
            contact->m_flags &= ~Contact::ON_ISLAND;
        });

        static std::vector<RigidBody*> body_stack;
        body_stack.reserve(m_bodies.count);

        m_solver.Initialize(dt, m_bodies.count, m_contacts.count);
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
            }

            m_solver.Solve();
            m_solver.ProcessPostSolve(*this);
        });

        m_bodies.for_each([=](auto* body) {
            // If a body was not in an island then it did not move.
            if (((body->m_flags & RigidBody::ON_ISLAND) == 0) ||
                (body->m_type == RigidBodyType::STATIC))
            {
                return;
            }

            body->SyncFixtures();

            if (m_flags & CLEAR_FORCES)
            {
                body->linear.force = { 0.f, 0.f };
                body->angular.torque = 0.f;
            }
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
    m_contacts.push_back(contact);
    body_a->contact_edges.push_back(&contact->edge_a);
    body_b->contact_edges.push_back(&contact->edge_b);

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

    m_contacts.remove(contact);
    body_a->contact_edges.remove(&contact->edge_a);
    body_b->contact_edges.remove(&contact->edge_b);

    block_allocator.Delete<Contact>(contact);
}

void
CollisionGraph::PurgeContacts (void)
{
    m_contacts.for_each([this](auto* contact) {
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
                                        [=](int32 h) { return h == handle; }));
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

#ifdef RDGE_DEBUG
void
CollisionGraph::DebugDraw (void)
{
    if (debug_flags & DRAW_BVH_NODES)
    {
        m_tree.DebugDraw();
    }

    if (debug_flags & DRAW_BODIES)
    {
        m_bodies.for_each([=](auto* body) {
            body->fixtures.for_each([=](auto* f) {
                if (debug_flags & DRAW_FIXTURES)
                {
                    if (!body->IsSimulating())
                    {
                        debug::DrawWireFrame(f, color(127, 127, 76));
                    }
                    else if (body->GetType() == RigidBodyType::STATIC)
                    {
                        debug::DrawWireFrame(f, color(127, 230, 127));
                    }
                    else if (body->GetType() == RigidBodyType::KINEMATIC)
                    {
                        debug::DrawWireFrame(f, color(127, 127, 230));
                    }
                    if (!body->IsAwake())
                    {
                        debug::DrawWireFrame(f, color(152, 152, 152));
                    }
                    else
                    {
                        debug::DrawWireFrame(f, color(230, 178, 178));
                    }
                }

                if (debug_flags & DRAW_AABBS)
                {
                    debug::DrawWireFrame(f->proxy->box, color(230, 76, 230));
                }

                if (debug_flags & DRAW_CENTER_OF_MASS)
                {

                }
            });
        });
    }
}
#endif

} // namespace physics
} // namespace rdge
