#include <rdge/physics/fixture.hpp>
#include <rdge/physics/rigid_body.hpp>
#include <rdge/physics/collision_graph.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>

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
        shape = allocator.New<circle>(*static_cast<const circle*>(profile.shape));
        break;

    case ShapeType::POLYGON:
        shape = allocator.New<polygon>(*static_cast<const polygon*>(profile.shape));
        break;

    default:
        SDL_assert(false);
        break;
    }

    proxy = allocator.Alloc<fixture_proxy>();
    proxy->fixture = this;
    proxy->box = body->world_transform.to_world(shape->compute_aabb());
    proxy->box.fatten();
}

Fixture::~Fixture (void) noexcept
{
    SDL_assert(shape != nullptr);
    SDL_assert(body != nullptr && body->graph != nullptr);

    auto& allocator = body->graph->block_allocator;
    allocator.Free<fixture_proxy>(proxy);
    proxy = nullptr;

    switch (shape->type())
    {
    case ShapeType::CIRCLE:
        allocator.Delete<circle>(static_cast<circle*>(shape));
        break;

    case ShapeType::POLYGON:
        allocator.Delete<polygon>(static_cast<polygon*>(shape));
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

} // namespace physics
} // namespace rdge
