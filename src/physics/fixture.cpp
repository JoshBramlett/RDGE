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
    , is_sensor(profile.is_sensor)
{
    SDL_assert(parent != nullptr);

    auto& allocator = body->graph->block_allocator;
    switch (this->shape->type())
    {
    case ShapeType::CIRCLE:
        this->shape = allocator.New<circle>(*static_cast<const circle*>(profile.shape));
        break;

    case ShapeType::POLYGON:
        this->shape = allocator.New<polygon>(*static_cast<const polygon*>(profile.shape));
        break;

    default:
        SDL_assert(false);
        break;
    }

    m_proxy = allocator.Alloc<fixture_proxy>();
    m_proxy->fixture = this;
    m_proxy->box = this->body->world_transform.to_world(this->shape->compute_aabb());
}

Fixture::~Fixture (void) noexcept
{
    SDL_assert(this->shape != nullptr);
    SDL_assert(this->body != nullptr && this->body->graph != nullptr);

    auto& allocator = this->body->graph->block_allocator;
    allocator.Free<fixture_proxy>(m_proxy);
    m_proxy = nullptr;

    switch (this->shape->type())
    {
    case ShapeType::CIRCLE:
        allocator.Delete<circle>(static_cast<circle*>(this->shape));
        break;

    case ShapeType::POLYGON:
        allocator.Delete<polygon>(static_cast<polygon*>(this->shape));
        break;

    default:
        SDL_assert(false);
        break;
    }
}

} // namespace physics
} // namespace rdge
