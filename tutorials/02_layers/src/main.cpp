#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/system/window.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>

// *** Resource shoutouts ***
//
// Font provided by /u/teryror
//  - https://www.reddit.com/r/gamedev/comments/3clk56
// Images provided by kenney.nl
//  - http://kenney.nl/assets/shooting-gallery

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "02_layers";
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
    auto duck = std::make_shared<Sprite>(math::vec3(-64.f, -64.f, 0.f), texture);
    auto attrib = std::make_shared<Text>("Thanks kenney.nl",
                                         math::vec3(-220.f, -150.f, 0.f),
                                         font,
                                         color::GREEN);

    // 5) Add sprites to the layer
    SpriteLayer layer;
    layer.AddSprite(duck);
    layer.AddSprite(attrib);

    // 6) Create game loop
    bool running = true;
    Event event;
    while (running)
    {
        // 7) Poll for user input
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

        // 8) Render to the screen using the layer
        window.Clear();

        layer.Draw();

        window.Present();
    }

    return 0;
}
