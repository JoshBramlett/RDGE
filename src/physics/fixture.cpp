#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/util/memory/small_block_allocator.hpp>

#include <SDL_assert.h>

namespace rdge {
namespace physics {

Fixture::Fixture (const fixture_profile& profile, RigidBody* parent)
    : body(parent)
    , user_data(profile.user_data)
    , density(profile.density)
    , friction(profile.friction)
    , restitution(profile.restitution)
    , filter(profile.filter)
{
    SDL_assert(parent != nullptr);

    if (profile.is_sensor)
    {
        m_flags |= SENSOR;
    }

    auto& allocator = body->graph->block_allocator;
    switch (profile.shape->type())
    {
    case ShapeType::CIRCLE:
        shape.local = allocator.New<circle>(*static_cast<const circle*>(profile.shape));
        shape.world = allocator.New<circle>(*static_cast<const circle*>(profile.shape));
        break;

    case ShapeType::POLYGON:
        shape.local = allocator.New<polygon>(*static_cast<const polygon*>(profile.shape));
        shape.world = allocator.New<polygon>(*static_cast<const polygon*>(profile.shape));
        break;

    default:
        SDL_assert(false);
        break;
    }

    shape.world->to_world(body->world_transform);
    proxy = allocator.Alloc<fixture_proxy>();
    proxy->fixture = this;
    proxy->handle = fixture_proxy::INVALID_HANDLE;
}

Fixture::~Fixture (void) noexcept
{
    SDL_assert(shape.local != nullptr);
    SDL_assert(shape.world != nullptr);
    SDL_assert(body != nullptr && body->graph != nullptr);

    auto& allocator = body->graph->block_allocator;
    allocator.Free<fixture_proxy>(proxy);

    switch (shape.local->type())
    {
    case ShapeType::CIRCLE:
        allocator.Delete<circle>(static_cast<circle*>(shape.local));
        allocator.Delete<circle>(static_cast<circle*>(shape.world));
        break;

    case ShapeType::POLYGON:
        allocator.Delete<polygon>(static_cast<polygon*>(shape.local));
        allocator.Delete<polygon>(static_cast<polygon*>(shape.world));
        break;

    default:
        SDL_assert(false);
        break;
    }
}

void
Fixture::SetSensor (bool value) noexcept
{
    if (IsSensor() != value)
    {
        body->WakeUp();

        if (value)
        {
            m_flags |= SENSOR;
        }
        else
        {
            m_flags &= ~SENSOR;
        }
    }
}

void
Fixture::Syncronize (void)
{
    switch (shape.local->type())
    {
    case ShapeType::CIRCLE:
        *static_cast<circle*>(shape.world) = *static_cast<const circle*>(shape.local);
        break;

    case ShapeType::POLYGON:
        *static_cast<polygon*>(shape.world) = *static_cast<const polygon*>(shape.local);
        break;

    default:
        SDL_assert(false);
        break;
    }

    shape.world->to_world(body->world_transform);
}

} // namespace physics
} // namespace rdge
