#include <rdge/physics/collision_graph.hpp>

namespace rdge {
namespace physics {

CollisionGraph::CollisionGraph (const math::vec2& g)
    : gravity(g)
{ }

CollisionGraph::~CollisionGraph (void) noexcept
{ }

RigidBody*
CollisionGraph::CreateBody (const rigid_body_profile& profile)
{
    // TODO IsLocked

    RigidBody* result = block_allocator.New<RigidBody>(profile, this);
    bodies.push_back(result);

    return result;
}

void
CollisionGraph::DestroyBody (RigidBody* body)
{
    // TODO IsLocked

    // TODO Destroy attached joints
    // TODO Destroy attached contacts
    // TODO Destroy attached fixtures

    bodies.remove(body);
    block_allocator.Delete<RigidBody>(body);
}

void
CollisionGraph::Step (float dt)
{
    Unused(dt);
}

void
CollisionGraph::CreateContact (Fixture* a, Fixture* b)
{
    if (a->body == b->body)
    {
        return;
    }

    if (a->body->ShouldCollide(b->body) == false)
    {
        return;
    }

    // TODO check if contact exists

    if (custom_filter && custom_filter->ShouldCollide(a, b) == false)
    {
        return;
    }
}

void
CollisionGraph::DestroyContact (Contact* contact)
{
    if (contact->IsTouching())
    {
        if (listener)
        {
            listener->EndContact(contact);
        }

        if (!contact->fixture_a->IsSensor() &&
            !contact->fixture_b->IsSensor())
        {
            contact->fixture_a->body->WakeUp();
            contact->fixture_b->body->WakeUp();
        }
    }

    // TODO not sure why, but the contact list is a double linked list
    contacts.remove(contact);

    // TODO remove contact from body 1
    // TODO remove contact from body 2

    // TODO dealloc
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

        if (a->proxy->box.intersects_with(b->proxy->box))
        {
            DestroyContact(contact);
            return;
        }

        contact->Update(listener);
    });
}

void
CollisionGraph::RegisterProxy (fixture_proxy* proxy)
{
    Unused(proxy);
}

void
CollisionGraph::UnregisterProxy (fixture_proxy* proxy)
{
    Unused(proxy);
}

} // namespace physics
} // namespace rdge
