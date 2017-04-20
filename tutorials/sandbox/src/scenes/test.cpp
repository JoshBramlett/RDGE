#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>
#include <rdge/util.hpp>

#include <rdge/debug.hpp>

#include <SDL_assert.h>

using namespace rdge;
using namespace rdge::math;

TestScene::TestScene (void)
    : duck(vec3::ZERO)
    , render_target(std::make_shared<SpriteBatch>(10'000))
    , background(render_target)
{
    SpriteSheet sheet("res/environment.json", Window::Current().IsHighDPI());

    ///////////////////
    // Background layer
    ///////////////////

    background.AddSprite(sheet.CreateSpriteChain("dirt",
                                                 vec3(-960.f, -540.f, 0.f),
                                                 vec2(1920.f, 1080.f)));

    Random rng;
    for (float y = -540.f; y < 540.f; y += 64.f)
    {
        for (float x = -960.f; x < 960.f; x += 64.f)
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

    background.AddSprite(player.sprite);
    background.AddSprite(duck.sprite);

    walls.left = aabb({ -990.f, -570.f }, { -960.f, 570.f });
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
    player.OnUpdate(dt);
    duck.OnUpdate(dt);
}

void
TestScene::OnRender (void)
{
    camera.Translate(player.user_input.position_offset);
    camera.Update();
    render_target->SetProjection(camera.combined);

    background.Draw();

    debug::SetLineColor(color::CYAN);
    aabb player_aabb(player.sprite->vertices[0].pos.xy(),
                     player.sprite->vertices[2].pos.xy());
    if (player_aabb.intersects_with(walls.left))
    {
        debug::DrawWireFrame(walls.left, color::RED);
    }
    else
    {
        debug::DrawWireFrame(walls.left);
    }

    //debug::DrawWireFrame(circle(vec2(100.f, 100.f), 50.f), color::BLUE);
    debug::DrawWireFrame(circle(vec2(100.f, 100.f), 50.f));

    debug::SetProjection(camera.combined);
}
