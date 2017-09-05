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

    game.on_event_hook = [&](const Event& event) {
        if (event.IsQuitEvent())
        {
            game.Stop();
        }
        else if (event.IsKeyboardEvent())
        {
            auto e = event.GetKeyboardEventArgs();
            if (e.Key() == KeyCode::ESCAPE)
            {
                game.Stop();
            }
        }

        return false;
    };

    game.PushScene(std::make_shared<TestScene>());
    //game.PushScene(std::make_shared<TilesScene>());
    //game.PushScene(std::make_shared<TumblerScene>());
    game.Run();

    return 0;
}
