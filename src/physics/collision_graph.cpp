#include <rdge/physics/collision_graph.hpp>

#include <algorithm> // remove_if

namespace rdge {
namespace physics {

CollisionGraph::CollisionGraph (const math::vec2& g)
    : gravity(g)
    , m_dirtyProxies(128)
{ }

CollisionGraph::~CollisionGraph (void) noexcept
{
    bodies.for_each([=](auto* b) {
        block_allocator.Delete<RigidBody>(b);
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

    // body dtor removes associated proxies, contacts and joints
    bodies.remove(body);
    block_allocator.Delete<RigidBody>(body);
}

void
CollisionGraph::Step (float dt)
{
    Unused(dt);
    m_flags |= LOCKED;

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
}

void
CollisionGraph::CreateContact (fixture_proxy* a, fixture_proxy* b)
{
    RigidBody* body_a = a->fixture->body;
    RigidBody* body_b = b->fixture->body;

    if (body_a == body_b)
    {
        return;
    }

    if (!body_a->ShouldCollide(body_b))
    {
        return;
    }

    if (!body_a->HasEdge(a->fixture, b->fixture))
    {
        return;
    }

    if (custom_filter && custom_filter->ShouldCollide(a->fixture, b->fixture) == false)
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
            listener->EndContact(contact);
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

        // purge non-intersecting contacts
        if (a->proxy->box.intersects_with(b->proxy->box))
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

} // namespace physics
} // namespace rdge
