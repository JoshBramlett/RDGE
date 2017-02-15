#include "test.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/util.hpp>
#include <rdge/system/window.hpp>

using namespace rdge;
using namespace rdge::math;

TestScene::TestScene (void)
    : render_target(std::make_shared<SpriteBatch>(10'000))
    , background(render_target)
{
    SpriteSheet env("res/environment.json", Window::Current().IsHighDPI());

    ///////////////////
    // Background layer
    ///////////////////

    background.AddSprite(env.CreateSpriteChain("dirt",
                                               vec3(-960.f, -540.f, 0.f),
                                               vec2(1920.f, 1080.f)));

    Random rng;
    for (float y = -540.f; y < 540.f; y += 128.f)
    {
        for (float x = -960.f; x < 960.f; x += 128.f)
        {
            auto random = rng.Next(15);
            if (random == 0)
            {
                background.AddSprite(env.CreateSprite("weed1", vec3(x, y, 0.f)));
            }
            else if (random == 1)
            {
                background.AddSprite(env.CreateSprite("rock1", vec3(x, y, 0.f)));
            }
        }
    }
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
TestScene::OnEvent (const Event&)
{ }

void
TestScene::OnUpdate (uint32)
{ }

void
TestScene::OnRender (void)
{
    camera.Update();
    render_target->SetProjection(camera.combined);

    background.Draw();
}
