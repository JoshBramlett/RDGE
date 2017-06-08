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
        m_shape = allocator.New<circle>(*static_cast<const circle*>(profile.shape));
        m_worldShape = allocator.Alloc<circle>();
        break;

    case ShapeType::POLYGON:
        m_shape = allocator.New<polygon>(*static_cast<const polygon*>(profile.shape));
        m_worldShape = allocator.Alloc<polygon>();
        break;

    default:
        SDL_assert(false);
        break;
    }

    proxy = allocator.Alloc<fixture_proxy>();
    proxy->fixture = this;
    proxy->handle = fixture_proxy::INVALID_HANDLE;
}

Fixture::~Fixture (void) noexcept
{
    SDL_assert(m_shape != nullptr);
    SDL_assert(body != nullptr && body->graph != nullptr);

    auto& allocator = body->graph->block_allocator;
    allocator.Free<fixture_proxy>(proxy);

    switch (m_shape->type())
    {
    case ShapeType::CIRCLE:
        allocator.Delete<circle>(static_cast<circle*>(m_shape));
        allocator.Free<circle>(static_cast<circle*>(m_worldShape));
        break;

    case ShapeType::POLYGON:
        allocator.Delete<polygon>(static_cast<polygon*>(m_shape));
        allocator.Free<polygon>(static_cast<polygon*>(m_worldShape));
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

ishape*
Fixture::GetWorldShape (void) noexcept
{
    switch (m_shape->type())
    {
    case ShapeType::CIRCLE:
        *static_cast<circle*>(m_worldShape) = *static_cast<const circle*>(m_shape);
        break;

    case ShapeType::POLYGON:
        *static_cast<polygon*>(m_worldShape) = *static_cast<const polygon*>(m_shape);
        break;

    default:
        SDL_assert(false);
        break;
    }

    m_worldShape->to_world(body->world_transform);
    return m_worldShape;
}

} // namespace physics
} // namespace rdge
