#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/system/window.hpp>

#include <memory>

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "02_textures";
    settings.enable_png   = true;
    settings.enable_fonts = true;

    // 1) Initialize SDL
    Application app(settings);

    // 2) Create window
    Window window(settings);

    // 3) Load assets
    auto font = std::make_shared<Font>("res/OpenSansPX.ttf", 64);
    auto texture = std::make_shared<Texture>("res/duck.png");

    // 4) Create renderable graphics
    Sprite duck({ -1.f, -1.f, 0.f }, { 2.f, 2.f }, texture);
    Text attribution("Thanks kenney.nl", { -3.5f, 2.f, 0.f }, font, color::GREEN);

    // 3) Create render target & register renderables
    SpriteBatch renderer;
    duck.SetRenderTarget(renderer);
    attribution.SetRenderTarget(renderer);

    // 4) Create game loop
    bool running = true;
    Event event;
    while (running)
    {
        // 5) Poll for user input
        while (PollEvent(&event))
        {
            if (event.IsQuitEvent())
            {
                running = false;
                break;
            }
            else if (event.IsKeyboardEvent())
            {
                auto e = event.GetKeyboardEventArgs();
                if (e.Key() == KeyCode::Escape)
                {
                    running = false;
                    break;
                }
            }
        }

        // 6) Render to the screen
        window.Clear();

        renderer.PrepSubmit();
        duck.Draw(renderer);
        attribution.Draw(renderer);
        renderer.Flush();

        window.Present();
    }

    return 0;
}
