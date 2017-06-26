#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

class TestSceneListener : public GraphListener
{
public:

    ~TestSceneListener (void) noexcept = default;

    void OnContactStart (Contact*) override
    {
        std::cout << "OnContactStart" << std::endl;
    }

    void OnContactEnd (Contact*) override
    {
        std::cout << "OnContactEnd" << std::endl;
    }

    void OnPreSolve (Contact*, const collision_manifold*) override { }
    void OnPostSolve (Contact*, const contact_impulse*) override { }
    void OnDestroyed (Fixture*) override { }
};

namespace {

    TestSceneListener l;

} // anonymous namespace

TestScene::TestScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    camera.zoom = 0.1f;

    collision_graph.listener = &l;

    rigid_body_profile bprof;
    fixture_profile fprof;

    bprof.type = RigidBodyType::STATIC;
    body_c = collision_graph.CreateBody(bprof);

    bprof.type = RigidBodyType::DYNAMIC;
    body_a = collision_graph.CreateBody(bprof);

    bprof.linear_velocity = { 6.5f, 0.f };
    //bprof.angular_velocity = 25.f;
    body_b = collision_graph.CreateBody(bprof);

    polygon floor({ -10.f, -10.f }, { -10.f, -11.f },
                  { 10.f, -11.f, }, { 10.f, -10.f });
    //polygon floor({ -100.f, -100.f }, { -100.f, -110.f },
                  //{ 100.f, -110.f, }, { 100.f, -100.f });
    fprof.shape = &floor;
    fixture_c = body_c->CreateFixture(fprof);

    //polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });
    //polygon tri_a({ 20.f, 70.f }, { 40.f, 20.f }, { 80.f, 70.f });
    polygon tri_a({ 2.f, 2.f }, { 2.f, 8.f }, { 8.f, 2.f }, { 8.f, 8.f });
    //polygon tri_a({ 20.f, 20.f }, { 20.f, 80.f }, { 80.f, 20.f }, { 80.f, 80.f });
    fprof.shape = &tri_a;
    fprof.density = 1.f;
    fprof.restitution = 0.5f;
    fixture_a = body_a->CreateFixture(fprof);

    //polygon tri_b({ -2.f, 7.f }, { -4.f, 2.f }, { -8.f, 7.f });
    //polygon tri_b({ -20.f, 70.f }, { -40.f, 20.f }, { -80.f, 70.f });
    polygon tri_b({ -2.f, 2.f }, { -2.f, 8.f }, { -8.f, 2.f }, { -8.f, 8.f });
    //polygon tri_b({ -20.f, 20.f }, { -20.f, 80.f }, { -80.f, 20.f }, { -80.f, 80.f });
    fprof.shape = &tri_b;
    fixture_b = body_b->CreateFixture(fprof);




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

    //body_b->linear.force = { 20000.2f, 0.f };
    //body->angular.torque = 200000.f;

    //collision_graph.Step(dt.seconds);
    collision_graph.Step(1.f / 60.f);
    collision_graph.Step(1.f / 60.f);
}

void
TestScene::OnRender (void)
{

    //debug::DrawWireFrame(fixture_a->proxy->box, color::WHITE);
    //debug::DrawWireFrame(fixture_b->proxy->box, color::WHITE);

    collision_graph.DebugDraw();

    auto tri_a = static_cast<polygon*>(fixture_a->shape.world);
    debug::DrawWireFrame(*tri_a);
    auto tri_b = static_cast<polygon*>(fixture_b->shape.world);
    debug::DrawWireFrame(*tri_b);
    auto floor = static_cast<polygon*>(fixture_c->shape.world);
    debug::DrawWireFrame(*floor);

    //aabb a({-280.11, -270.11}, {-219.89, -219.89});
    //aabb b({19.89, 19.89}, {80.11, 70.11});
    //debug::DrawWireFrame(a);
    //debug::DrawWireFrame(b);

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
