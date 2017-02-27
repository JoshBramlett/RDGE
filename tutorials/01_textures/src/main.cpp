#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/system/window.hpp>

#include <rdge/assets.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/util.hpp>

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
    try
    {
        app_settings settings;
        settings.window_title = "01_textures";
        settings.enable_png   = true;
        settings.enable_fonts = true;

        // 1) Initialize SDL
        Application app(settings);

        // (Optional) Disable unprocessed events
        DisableEvent(EventType::FingerDown);
        DisableEvent(EventType::FingerUp);
        DisableEvent(EventType::FingerMotion);
        DisableEvent(EventType::MultiGesture);

        // 2) Create window
        Window window(settings);

        // 3) Load assets
        auto font = std::make_shared<Font>("res/OpenSansPX.ttf", 64);
        auto texture = std::make_shared<Texture>("res/duck.png");

        // 4) Create renderable graphics
        Sprite duck({ -64.f, -64.f, 0.f }, { 128.f, 128.f }, texture);
        Text attrib("Thanks kenney.nl", { -220.f, -150.f, 0.f }, font, color::GREEN);

        // 3) Create render target & register renderables
        SpriteBatch renderer;
        duck.SetRenderTarget(renderer);
        attrib.SetRenderTarget(renderer);

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
                    if (e.Key() == KeyCode::ESCAPE)
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
            attrib.Draw(renderer);
            renderer.Flush();

            window.Present();
        }
    }
    catch (const rdge::SDLException& ex)
    {
        std::cerr << "SDLException Raised!"
                  << "\n    " << ex.what()
                  << "\n    " << ex.SDLFunction()
                  << "\n    " << ex.SDLError() << "\n\n";
    }
    catch (const rdge::GLException& ex)
    {
        std::cerr << "GLException Raised!"
                  << "\n    " << ex.what()
                  << "\n    " << ex.GLFunction()
                  << "\n    " << ex.GLErrorCodeString() << "\n\n";
    }
    catch (const rdge::Exception& ex)
    {
        std::cerr << "Exception Raised!"
                  << "\n    " << ex.what()
                  << "\n    " << ex.File()
                  << "\n    " << ex.FileName() << ":" << ex.Line()
                  << " [" << ex.Function() << "]\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n\n";
    }

    return 0;
}
