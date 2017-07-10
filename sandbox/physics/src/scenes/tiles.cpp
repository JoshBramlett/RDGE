#include "tiles.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TilesScene::TilesScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    camera.zoom = 0.03f;

    collision_graph.debug_flags = CollisionGraph::DRAW_FIXTURES;

    {
        float a = 0.5f;
        rigid_body_profile bprof;
        bprof.position.y = -a;
        bprof.type = RigidBodyType::DYNAMIC;//STATIC;
        auto ground = collision_graph.CreateBody(bprof);

        size_t N = 2; //200;
        size_t M = 1;
        math::vec2 position;
        for (size_t j = 0; j < M; j++)
        {
            position.x = -N * a;
            for (size_t i = 0; i < N; i++)
            {
                polygon p(a, a, position);
                ground->CreateFixture(&p, 0.f);
                position.x += 2.f * a;
            }

            position.y -= 2.f * a;
        }
    }

    //polygon side_a(0.5f, 10.f, { 10.f, 0.f }, 0.f);
    //tumbler->CreateFixture(&side_a, 5.f);
    //polygon side_b(0.5f, 10.f, { -10.f, 0.f }, 0.f);
    //tumbler->CreateFixture(&side_b, 5.f);
    //polygon side_c(10.f, 0.5f, { 0.f, 10.f }, 0.f);
    //tumbler->CreateFixture(&side_c, 5.f);
    //polygon side_d(10.f, 0.5f, { 0.f, -10.f }, 0.f);
    //tumbler->CreateFixture(&side_d, 5.f);
}

void
TilesScene::Initialize (void)
{ }

void
TilesScene::Terminate (void)
{ }

void
TilesScene::Hibernate (void)
{ }

void
TilesScene::Activate (void)
{ }

void
TilesScene::OnEvent (const Event& event)
{
    Unused(event);
}

void
TilesScene::OnUpdate (const delta_time& dt)
{
    Unused(dt);

    collision_graph.Step(1.f / 60.f);
    collision_graph.Step(1.f / 60.f);
}

void
TilesScene::OnRender (void)
{
    collision_graph.DebugDraw(1.f);

    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
