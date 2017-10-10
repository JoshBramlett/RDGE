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

    void OnContactStart (Contact*) override { }
    void OnContactEnd (Contact*) override { }

    void OnPreSolve (Contact*, const collision_manifold&) override { }
    void OnPostSolve (Contact*) override { }
    void OnDestroyed (Fixture*) override { }
};

namespace {

    TestSceneListener l;

} // anonymous namespace

TestScene::TestScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    collision_graph.listener = &l;
    debug::settings::draw_physics_joints = true;
}

void
TestScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
    camera.zoom = 0.05f;

    // bodies
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::STATIC;
    body_c = collision_graph.CreateBody(bprof);

    bprof.type = RigidBodyType::DYNAMIC;
    body_a = collision_graph.CreateBody(bprof);

    bprof.linear_velocity = { 6.5f, 0.f };
    body_b = collision_graph.CreateBody(bprof);


    // fixtures
    fixture_profile fprof;
    polygon floor({ -10.f, -10.f }, { -10.f, -11.f },
                  { 10.f, -11.f, }, { 10.f, -10.f });
    fprof.shape = &floor;
    fprof.density = 1.f;
    fprof.restitution = 0.5f;
    fixture_c = body_c->CreateFixture(fprof);

    //polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });
    polygon box_a({ 2.f, 2.f }, { 2.f, 8.f }, { 8.f, 2.f }, { 8.f, 8.f });
    fprof.shape = &box_a;
    fixture_a = body_a->CreateFixture(fprof);

    //polygon tri_b({ -2.f, 7.f }, { -4.f, 2.f }, { -8.f, 7.f });
    polygon box_b({ -2.f, 2.f }, { -2.f, 8.f }, { -8.f, 2.f }, { -8.f, 8.f });
    fprof.shape = &box_b;
    fixture_b = body_b->CreateFixture(fprof);
}

void
TestScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);

    collision_graph.ClearGraph();
}

void
TestScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
}

void
TestScene::Hibernate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
TestScene::OnEvent (const Event& event)
{
    Unused(event);
}

void
TestScene::OnUpdate (const delta_time& dt)
{
    Unused(dt);

    collision_graph.Step(1.f / 60.f);
}

void
TestScene::OnRender (void)
{
    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
