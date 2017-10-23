#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>
#include <rdge/debug.hpp>

#include <SDL_assert.h>

#include "../asset_enums.hpp"
#include "../globals.hpp"

#define CHRONO_ADD_WALLS 0

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

class TestSceneListener : public GraphListener
{
public:

    ~TestSceneListener (void) noexcept = default;

    void OnContactStart (Contact*) override
    {
        //std::cout << "OnContactStart" << std::endl;
    }

    void OnContactEnd (Contact*) override
    {
        //std::cout << "OnContactEnd" << std::endl;
    }

    void OnPreSolve (Contact*, const collision_manifold&) override
    {
        //std::cout << "OnPreSolve" << std::endl;
    }

    void OnPostSolve (Contact* c) override
    {
        rdge::Unused(c);
        //std::cout << "OnPostSolve" << std::endl
                  //<< c->manifold << std::endl
                  //<< c->impulse << std::endl;
    }

    void OnDestroyed (Fixture*) override { }
};

namespace {

    TestSceneListener l;

} // anonymous namespace

TestScene::TestScene (void)
    : collision_graph({ 0.f, -9.8f })
    , duck(this, vec3(300.f, 300.f, 0.f)) //vec3::ZERO)
    , render_target(std::make_shared<SpriteBatch>(10'000))
    , entities(render_target)
{
    collision_graph.listener = &l;
    debug::settings::show_overlay = true;

    ///////////////////
    // Background layer
    ///////////////////

    // TODO config file?
    //float window_height = 1080.f;
    //float window_width  = 1920.f;
    //float half_height   = window_height * 0.5f;
    //float half_width    = window_width * 0.5f;

    // TODO Think about how to properly handle conversion from physics simulation
    //      coordinates to rendering coordinates.
    //        - Should everything be defined in simulation coordinates, then do the
    //          conversion at the render phase?
    //        - Should only entities in the simulation be defined in simulation
    //          coordinates, having each entity (simulation or no) handle their
    //          own rendering?

    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_tilemap_crossroads);
    background = TilemapBatch(sheet, 4.f);

#if (CHRONO_ADD_WALLS)
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::STATIC;
    auto walls = collision_graph.CreateBody(bprof);

    float pad = 0.05f;
    float sim_half_width = half_width * INV_PIXELS_PER_METER;
    float sim_half_height = half_height * INV_PIXELS_PER_METER;

    polygon left_wall(pad, sim_half_height + pad + pad,
                      { -(sim_half_width + pad), 0.f });
    walls->CreateFixture(&left_wall, 0.f);

    polygon right_wall(pad, sim_half_height + pad + pad,
                       { sim_half_width + pad, 0.f });
    walls->CreateFixture(&right_wall, 0.f);

    polygon top_wall(sim_half_width + pad + pad, pad,
                     { 0.f, sim_half_height + pad });
    walls->CreateFixture(&top_wall, 0.f);

    polygon bottom_wall(sim_half_width + pad + pad, pad,
                        { 0.f, -(sim_half_height + pad) });
    walls->CreateFixture(&bottom_wall, 0.f);
#endif

    player.InitPhysics(collision_graph, INV_PIXELS_PER_METER);
    duck.InitPhysics(collision_graph, INV_PIXELS_PER_METER);

    entities.AddSprite(player.sprite);
    entities.AddSprite(duck.sprite);
}

void
TestScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, PIXELS_PER_METER);
}

void
TestScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
TestScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, PIXELS_PER_METER);
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
    player.OnEvent(event);
}

void
TestScene::OnUpdate (const delta_time& dt)
{
    collision_graph.Step(1.f / 60.f);
    player.OnUpdate(dt);
    duck.OnUpdate(dt);
}

void
TestScene::OnRender (void)
{
    camera.SetPosition(player.GetWorldCenter() * PIXELS_PER_METER);
    camera.Update();

    render_target->SetProjection(camera.combined);
    background.SetView(camera);

    background.Draw();
    entities.Draw();

    // debug drawing
    debug::SetProjection(camera.combined);
}
