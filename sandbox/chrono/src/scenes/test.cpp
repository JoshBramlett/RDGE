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
    : collision_graph({ 0.f, -9.8f })
    , duck(this, vec3::ZERO)
    , render_target(std::make_shared<SpriteBatch>(10'000))
    , background(render_target)
    , entities(render_target)
{
    SpriteSheet sheet("res/environment.json", Window::Current().IsHighDPI());

    ///////////////////
    // Background layer
    ///////////////////

    // TODO config file?
    float window_height = 1080.f;
    float window_width  = 1920.f;
    float half_height   = window_height * 0.5f;
    float half_width    = window_width * 0.5f;

    // TODO Think about how to properly handle conversion from physics simulation
    //      coordinates to rendering coordinates.
    //        - Should everything be defined in simulation coordinates, then do the
    //          conversion at the render phase?
    //        - Should only entities in the simulation be defined in simulation
    //          coordinates, having each entity (simulation or no) handle their
    //          own rendering?

#if 1
    background.AddSprite(sheet.CreateSpriteChain("dirt",
                                                 vec3(-half_width, -half_height, 0.f),
                                                 vec2(window_width, window_height)));

    // 1920.f -> 30
    // 1080.f -> 16.875
    Random rng;
    for (float y = -half_height; y < half_height; y += PIXELS_PER_METER)
    {
        for (float x = -half_width; x < half_width; x += PIXELS_PER_METER)
        {
            auto random = rng.Next(15);
            if (random == 0)
            {
                background.AddSprite(sheet.CreateSprite("weed1", vec3(x, y, 0.f)));
            }
            else if (random == 1)
            {
                background.AddSprite(sheet.CreateSprite("rock1", vec3(x, y, 0.f)));
            }
        }
    }
#else
    //collision_graph.debug_flags = CollisionGraph::DRAW_ALL;
    collision_graph.debug_flags = CollisionGraph::DRAW_FIXTURES;
#endif

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

    player.InitPhysics(collision_graph, INV_PIXELS_PER_METER);
    duck.InitPhysics(collision_graph, INV_PIXELS_PER_METER);

    entities.AddSprite(player.sprite);
    entities.AddSprite(duck.sprite);

    //camera.zoom = 1.5f;
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
    debug::DrawWireFrame(aabb({ -960.f, -540.f}, { 960.f, 540.f }), color::RED);

    math::vec2 t = { camera.position.x, camera.position.y };
    camera.Translate((player.GetWorldCenter() * PIXELS_PER_METER) - t);

    //camera.SetPosition(player.GetWorldCenter() * PIXELS_PER_METER);
    camera.Update();

    render_target->SetProjection(camera.combined);
    debug::SetProjection(camera.combined);

    background.Draw();
    entities.Draw();
    collision_graph.DebugDraw(PIXELS_PER_METER);
}