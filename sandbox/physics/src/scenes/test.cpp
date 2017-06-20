#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TestScene::TestScene (void)
    : collision_graph({ 0.f, 9.8f })
{
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::DYNAMIC;
    RigidBody* body = collision_graph.CreateBody(bprof);

    polygon tri_a({ 20.f, 70.f }, { 40.f, 20.f }, { 80.f, 70.f });
    fixture_profile fprof;
    fprof.shape = &tri_a;
    fprof.density = 1.f;
    fixture_a = body->CreateFixture(fprof);

    circle cir_a(vec2(100.f, 100.f), 50.f);
    fprof.shape = &cir_a;
    fixture_b = body->CreateFixture(fprof);
}

void
TestScene::Initialize (void)
{ }

void
TestScene::Terminate (void)
{ }

void
TestScene::Hibernate (void)
{ }

void
TestScene::Activate (void)
{ }

void
TestScene::OnEvent (const Event& event)
{
    Unused(event);
}

void
TestScene::OnUpdate (const delta_time& dt)
{
    Unused(dt);
}

void
TestScene::OnRender (void)
{
    collision_graph.Step(1.f / 60.f);

    debug::DrawWireFrame(fixture_a->proxy->box);
    debug::DrawWireFrame(fixture_b->proxy->box);

    //auto tri_a = static_cast<polygon*>(fixture_a->m_worldShape);
    //debug::DrawWireFrame(*tri_a);
    //auto cir_a = static_cast<circle*>(fixture_b->m_worldShape);
    //debug::DrawWireFrame(*cir_a);

/*
    polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });
    polygon tri_b({ 5.f, 9.f }, { 9.f, 3.f }, { 11.f, 10.f });

    static bool once = true;
    if (once)
    {
        collision_manifold mf;
        bool result = tri_a.intersects_with(tri_b, mf);

        std::cout << "tri_a:" << tri_a
                  << "tri_b:" << tri_b
                  << "result=" << std::boolalpha << result << "\n"
                  << "mf:" << mf;

        once = false;
    }

    debug::DrawWireFrame(tri_a);
    debug::DrawWireFrame(tri_b);

    polygon::PolygonData data;
    data[0] = {0.f, 0.f};
    data[1] = {150.f, 300.f};
    data[2] = {0.f, 300.f};
    auto p = polygon(data, 3);
    debug::DrawWireFrame(p);

    //debug::DrawWireFrame(circle(vec2(100.f, 100.f), 50.f), color::BLUE);
    //auto c = circle(vec2(100.f, 100.f), 50.f);
    //debug::DrawWireFrame(c);

    math::vec2 axis({ 0.707106781187f, 0.707106781187f });
    auto proj = p.project(axis);
    auto start = axis * proj.x;
    auto end = axis * proj.y;

    math::vec2 trans = { -200.f, 200.f };
    debug::DrawLine(trans + start, trans + end);

*/
    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
