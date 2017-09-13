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
    debug::settings::draw_physics_fixtures = true;
}

void
TilesScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
    camera.zoom = 0.03f;

    {
        float a = 0.5f;
        rigid_body_profile bprof;
        bprof.position.y = -a;
        bprof.type = RigidBodyType::STATIC;
        auto ground = collision_graph.CreateBody(bprof);

        //int32 N = 200;
        //int32 M = 10;
        int32 N = 50;
        int32 M = 3;
        math::vec2 position;
        for (int32 j = 0; j < M; j++)
        {
            position.x = -N * a;
            for (int32 i = 0; i < N; i++)
            {
                polygon p(a, a, position);
                ground->CreateFixture(&p, 0.f);
                position.x += 2.f * a;
            }

            position.y -= 2.f * a;
        }
    }

#if 1
    {
        int32 e_count = 2;
        float a = 0.5f;
        polygon shape(a, a);

        math::vec2 x(-7.0f, 0.75f);
        math::vec2 y;
        math::vec2 deltaX(0.5625f, 1.25f);
        math::vec2 deltaY(1.125f, 0.0f);

        for (int32 i = 0; i < e_count; ++i)
        {
            y = x;

            for (int32 j = i; j < e_count; ++j)
            {
                rigid_body_profile bprof;
                bprof.type = RigidBodyType::DYNAMIC;
                bprof.position = y;

                RigidBody* body = collision_graph.CreateBody(bprof);
                body->CreateFixture(&shape, 5.0f);
                y += deltaY;
            }

            x += deltaX;
        }
    }
#endif
}

void
TilesScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);

    collision_graph.ClearGraph();
}

void
TilesScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
}

void
TilesScene::Hibernate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

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
}

void
TilesScene::OnRender (void)
{
    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
