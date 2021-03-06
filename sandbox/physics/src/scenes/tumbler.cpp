#include "tumbler.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TumblerScene::TumblerScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    debug::settings::physics::draw_fixtures = true;
}

void
TumblerScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
    camera.zoom = 0.03f;

    m_count = 0;

    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    bprof.angular_velocity = 0.05f * math::PI;
    bprof.gravity_scale = 0.f;
    tumbler = collision_graph.CreateBody(bprof);

    polygon side_a(0.5f, 10.f, { 10.f, 0.f }, 0.f);
    tumbler->CreateFixture(&side_a, 5.f);
    polygon side_b(0.5f, 10.f, { -10.f, 0.f }, 0.f);
    tumbler->CreateFixture(&side_b, 5.f);
    polygon side_c(10.f, 0.5f, { 0.f, 10.f }, 0.f);
    tumbler->CreateFixture(&side_c, 5.f);
    polygon side_d(10.f, 0.5f, { 0.f, -10.f }, 0.f);
    tumbler->CreateFixture(&side_d, 5.f);
}

void
TumblerScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);

    collision_graph.ClearGraph();
}

void
TumblerScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
}

void
TumblerScene::Hibernate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
TumblerScene::OnEvent (const Event& event)
{
    Unused(event);
}

void
TumblerScene::OnUpdate (const delta_time& dt)
{
    tumbler->angular.velocity = 0.05f * math::PI;
    tumbler->linear.velocity = { 0.f, 0.f };

    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    if (m_count < 100)
    {
        static int32 spawn_timer = 0;
        spawn_timer += dt.ticks;
        if (spawn_timer > 100)
        {
            auto body = collision_graph.CreateBody(bprof);

            polygon box(0.125f, 0.125f);
            body->CreateFixture(&box, 1.f);

            spawn_timer = 0;
            m_count++;
        }
    }

    collision_graph.Step(1.f / 60.f);
}

void
TumblerScene::OnRender (void)
{
    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
