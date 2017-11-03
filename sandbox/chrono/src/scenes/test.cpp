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

TestScene::TestScene (void)
    : collision_graph({ 0.f, -9.8f })
    , duck(this)
    , duck2(this)
    , render_target(std::make_shared<SpriteBatch>(10'000))
    , entities(render_target)
{
    collision_graph.listener = this;
    debug::settings::show_overlay = true;
    debug::settings::draw_physics_fixtures = true;

    ///////////////////
    // Background layer
    ///////////////////

    auto sheet = g_game.pack->GetSpriteSheet(chrono_asset_tilemap_crossroads);
    background = TilemapBatch(sheet, 4.f);

#if (CHRONO_ADD_WALLS)
    rigid_body_profile bprof;
    bprof.type = RigidBodyType::STATIC;
    auto walls = collision_graph.CreateBody(bprof);

    float pad = 0.05f;
    float sim_half_width = half_width * g_game.inv_ppm;
    float sim_half_height = half_height * g_game.inv_ppm;

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

    player.InitPhysics(collision_graph, math::vec2(30.f, -30.f));
    duck.InitPhysics(collision_graph, math::vec2(30.f, -33.f));
    duck2.InitPhysics(collision_graph, math::vec2(33.f, -30.f));
    dove.InitPhysics(collision_graph, math::vec2(0.f, 0.f));

    entities.AddSprite(player.sprite);
    entities.AddSprite(duck.sprite);
    entities.AddSprite(duck2.sprite);
    entities.AddSprite(dove.sprite);
}

void
TestScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ppm);
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
    debug::RegisterPhysics(&collision_graph, g_game.ppm);
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
    auto bounds = camera.bounds;
    bounds.lo *= g_game.inv_ppm;
    bounds.hi *= g_game.inv_ppm;

    if (!dove.is_flying)
    {
        static Random rand;
        if (rand.Next(600) == 0) // approx every 10 seconds
        {
            float x = bounds.right() + 2.f;
            float y = bounds.top() - (bounds.height() * rand.Sample());
            dove.Spawn(math::vec2(x, y));
        }
    }
    else
    {
        // bounds must be larger than the spawn point
        bounds.fatten(3.f);
        if (!bounds.contains(dove.GetWorldCenter()))
        {
            dove.Disable();
        }
    }

    collision_graph.Step(1.f / 60.f);
    player.OnUpdate(dt);
    duck.OnUpdate(dt);
    duck2.OnUpdate(dt);
    dove.OnUpdate(dt);
}

void
TestScene::OnRender (void)
{
    camera.SetPosition(player.GetWorldCenter() * g_game.ppm);
    camera.Update();

    render_target->SetProjection(camera.combined);
    background.SetView(camera);

    //background.Draw();
    entities.Draw();

    // debug drawing
    debug::SetProjection(camera.combined);
}

void
TestScene::OnContactStart (Contact*)
{
    //std::cout << "OnContactStart" << std::endl;
}

void
TestScene::OnContactEnd (Contact*)
{
    //std::cout << "OnContactEnd" << std::endl;
}

void
TestScene::OnPreSolve (Contact*, const collision_manifold&)
{
    //std::cout << "OnPreSolve" << std::endl;
}

void
TestScene::OnPostSolve (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnPostSolve" << std::endl
              //<< c->manifold << std::endl
              //<< c->impulse << std::endl;
}

void
TestScene::OnDestroyed (Fixture*)
{ }
