#include "revolute.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

class RevoluteSceneListener : public GraphListener
{
public:

    ~RevoluteSceneListener (void) noexcept = default;

    void OnContactStart (Contact*) override { }
    void OnContactEnd (Contact*) override { }

    void OnPreSolve (Contact*, const collision_manifold&) override { }
    void OnPostSolve (Contact*) override { }
    void OnDestroyed (Fixture*) override { }
};

namespace {

    RevoluteSceneListener l;

} // anonymous namespace

RevoluteScene::RevoluteScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    collision_graph.listener = &l;
}

void
RevoluteScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
    debug::settings::draw_physics_joints = true;
    camera.zoom = 0.05f;

    rigid_body_profile gp;
    RigidBody* ground = collision_graph.CreateBody(gp);

    polygon ground_shape({ -40.f, -1.f }, { -40.f, 0.f },
                         { 40.f, -1.f, }, { 40.f, 0.f });
    ground->CreateFixture(&ground_shape, 1.f);

    {
        rigid_body_profile bprof;
        bprof.type = RigidBodyType::DYNAMIC;
        bprof.position = math::vec2(-10.f, 20.f);

        ball = collision_graph.CreateBody(bprof);
        auto c = circle(0.5f);
        ball->CreateFixture(&c, 5.f);

        float w = 100.f;
        ball->linear.velocity = math::vec2(-8.f * w, 0.f);
        ball->angular.velocity = w;

        joint = collision_graph.CreateRevoluteJoint(ground, ball, vec2(-10.f, 12.f));
        joint->SetMotorSpeed(1 * math::PI);
        joint->SetMaxMotorTorque(10000.f);
        joint->SetLimits(-0.25f * math::PI, 0.5f * math::PI);

        joint->EnableLimits();
    }

    ILOG() << *ball;
    ILOG() << *joint;
}

void
RevoluteScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);

    collision_graph.ClearGraph();
}

void
RevoluteScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph);
}

void
RevoluteScene::Hibernate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
RevoluteScene::OnEvent (const Event& event)
{
    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating() || !args.IsKeyPressed())
        {
            return;
        }

        if (args.PhysicalKey() == ScanCode::L)
        {
            // note: limits are broken
            if (joint->IsLimitsEnabled())
            {
                joint->DisableLimits();
            }
            else
            {
                joint->EnableLimits();
            }
        }
        else if (args.PhysicalKey() == ScanCode::M)
        {
            if (joint->IsMotorEnabled())
            {
                joint->DisableMotor();
            }
            else
            {
                joint->EnableMotor();
            }
        }
    }
}

void
RevoluteScene::OnUpdate (const delta_time& dt)
{
    Unused(dt);

    collision_graph.Step(1.f / 60.f);
    ILOG() << *ball;
}

void
RevoluteScene::OnRender (void)
{
    camera.Translate({ 0.f, 0.f });
    camera.Update();
    debug::SetProjection(camera.combined);
}
