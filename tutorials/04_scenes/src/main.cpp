#include <rdge/core.hpp>
#include <rdge/application.hpp>

#include <rdge/gameobjects/game.hpp>
#include <rdge/gameobjects/iscene.hpp>

#include <rdge/events/event.hpp>
#include <rdge/system/window.hpp>

#include <rdge/assets.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>

#include "shooting_gallery_scene.hpp"

#include <memory>

// *** Resource shoutouts ***
//
// Images provided by kenney.nl
//  - http://kenney.nl/assets/shooting-gallery

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "04_scenes";
    settings.enable_png   = true;

    // 1) Initialize SDL
    Application app(settings);

    // (Optional) Disable unprocessed events
    DisableEvent(EventType::FingerDown);
    DisableEvent(EventType::FingerUp);
    DisableEvent(EventType::FingerMotion);
    DisableEvent(EventType::MultiGesture);

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
            if (e.Key() == KeyCode::Escape)
            {
                game.Stop();
            }
        }

        return false;
    };

    game.PushScene(std::make_shared<ShootingGalleryScene>());
    game.Run();

    return 0;
}
