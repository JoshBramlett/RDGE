#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/sprite_batch.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/system/window.hpp>

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "01_hello_rect";

    // 1) Initialize SDL
    Application app(settings);

    // 2) Create window
    Window window(settings);

    // 3) Create render target & graphics
    SpriteBatch renderer;
    Sprite green_square({ -3.f, -3.f, 0.f }, { 6.f, 6.f }, color::GREEN);

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
        green_square.Draw(renderer);
        renderer.Flush();

        window.Present();
    }

    return 0;
}
