#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>

#include <chrono/globals.hpp>
#include <chrono/scenes/overworld.hpp>
#include <chrono/scenes/winery.hpp>

#include <memory>

chrono_globals g_game;

using namespace rdge;

int main ()
{
    app_settings settings;
    //settings.target_width = 1920;
    //settings.target_height = 1080;
    settings.window_title = "sandbox: chrono";
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
            const auto& args = event.GetKeyboardEventArgs();
            if (args.Key() == KeyCode::ESCAPE)
            {
                game.Stop();
            }
        }
        else if (event.IsCustomEvent())
        {
            const auto& args = event.GetCustomEventArgs();
            if (args.Type() == g_game.custom_events.push_scene)
            {
                game.PushScene(std::make_shared<OverworldScene>());
            }
        }

        return false;
    };

    try
    {
        PackFile pack("res/assets.pack");
        g_game.game = &game;
        g_game.pack = &pack;
        g_game.ratios = game_ratios(16.f, 2.f, ((game.window->IsHighDPI()) ? 2.f : 1.f));

        g_game.custom_events.push_scene = rdge::RegisterCustomEvent();
        g_game.custom_events.pop_scene = rdge::RegisterCustomEvent();

        //game.PushScene(std::make_shared<OverworldScene>());
        game.PushScene(std::make_shared<WineryScene>());
        game.Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
