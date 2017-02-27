#include <rdge/core.hpp>
#include <rdge/application.hpp>

#include <rdge/gameobjects/game.hpp>
#include <rdge/gameobjects/iscene.hpp>

#include <rdge/events/event.hpp>
#include <rdge/system/window.hpp>

#include <rdge/assets.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>

#include "scenes/test.hpp"

#include <memory>

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "sandbox";
    settings.enable_png   = true;

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
    game.Run();

    return 0;
}
