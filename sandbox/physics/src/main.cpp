#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>

#include "scenes/test.hpp"
#include "scenes/tiles.hpp"
#include "scenes/tumbler.hpp"

#include <memory>

using namespace rdge;

int main ()
{
    app_settings settings;
    //settings.target_width = 1920;
    //settings.target_height = 1080;
    settings.window_title = "sandbox: physics";
    settings.resizable    = true;

    // 1) Initialize SDL
    Application app(settings);

    // (Optional) Disable unprocessed events
    DisableEvent(EventType::FingerDown);
    DisableEvent(EventType::FingerUp);
    DisableEvent(EventType::FingerMotion);
    DisableEvent(EventType::MultiGesture);
    DisableEvent(EventType::TextInput);

    // 2) Create game object
    Game game(settings);

    auto test_scene = std::make_shared<TestScene>();
    auto tiles_scene = std::make_shared<TilesScene>();
    auto tumbler_scene = std::make_shared<TumblerScene>();

    game.on_event_hook = [&](const Event& event) {
        if (event.IsQuitEvent())
        {
            game.Stop();
        }
        else if (event.IsKeyboardEvent())
        {
            auto e = event.GetKeyboardEventArgs();
            if (e.IsRepeating() || !e.IsKeyPressed())
            {
                return false;
            }

            if (e.Key() == KeyCode::ESCAPE)
            {
                game.Stop();
            }
            else if (e.Key() == KeyCode::A)
            {
                game.SwapScene(test_scene);
            }
            else if (e.Key() == KeyCode::S)
            {
                game.SwapScene(tiles_scene);
            }
            else if (e.Key() == KeyCode::D)
            {
                game.SwapScene(tumbler_scene);
            }
        }

        return false;
    };

    game.PushScene(test_scene);
    game.Run();

    return 0;
}
