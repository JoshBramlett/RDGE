#include <rdge/physics/collision_graph.hpp>

namespace rdge {
namespace physics {

RigidBody*
CollisionGraph::CreateBody (const rigid_body_profile& profile)
{
    // TODO IsLocked

    RigidBody* result = block_allocator.New<RigidBody>(profile, this);

    result->prev = nullptr;
    result->next = bodies;
    if (bodies)
    {
        bodies->prev = result;
    }
    bodies = result;
    body_count++;

    return result;
}

void
CollisionGraph::DestroyBody (RigidBody* body)
{
    // TODO IsLocked

    // TODO Destroy attached joints
    // TODO Destroy attached contacts
    // TODO Destroy attached fixtures

    // Remove world body list.
    if (body->prev)
    {
        body->prev->next = body->next;
    }

    if (body->next)
    {
        body->next->prev = body->prev;
    }

    if (body == bodies)
    {
        bodies = body->next;
    }

    body_count--;
    block_allocator.Delete<RigidBody>(body);
}

} // namespace physics
} // namespace rdge
