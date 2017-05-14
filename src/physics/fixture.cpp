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

    auto& block_allocator = body->graph->block_allocator;
    switch (this->shape->type())
    {
    case ShapeType::CIRCLE:
        {
            void* cursor = block_allocator.Alloc(sizeof(circle));
            this->shape = new (cursor) circle;
            *this->shape = *static_cast<const circle*>(profile.shape);
        }
        break;

    case ShapeType::POLYGON:
        {
            void* cursor = block_allocator.Alloc(sizeof(polygon));
            this->shape = new (cursor) polygon;
            *this->shape = *static_cast<const polygon*>(profile.shape);
        }
        break;

    default:
        SDL_assert(false);
        break;
    }

    // TODO create proxy
}

Fixture::~Fixture (void) noexcept
{
    SDL_assert(this->shape != nullptr);
    SDL_assert(this->body != nullptr && this->body->graph != nullptr);

    auto& block_allocator = this->body->graph->block_allocator;
    switch (this->shape->type())
    {
    case ShapeType::CIRCLE:
        {
            circle* s = static_cast<circle*>(this->shape);
            s->~circle();
            block_allocator.Free(s, sizeof(circle));
        }
        break;

    case ShapeType::POLYGON:
        {
            polygon* s = static_cast<polygon*>(this->shape);
            s->~polygon();
            block_allocator.Free(s, sizeof(polygon));
        }
        break;

    default:
        SDL_assert(false);
        break;
    }
}

} // namespace physics
} // namespace rdge
