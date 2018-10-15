#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/gameobjects/game.hpp>
#include <rdge/system/window.hpp>

#include <chrono/globals.hpp>
#include <chrono/types.hpp>
#include <chrono/events.hpp>
#include <chrono/scenes/scene_manager.hpp>


chrono_globals g_game;

using namespace rdge;

int main ()
{
    app_settings settings;
    //settings.target_width = 1920;
    //settings.target_height = 1080;
    settings.window_title = "sandbox: chrono";
    settings.resizable    = true;
    settings.use_vsync    = false;

    // 1) Initialize SDL
    Application app(settings);
    perch::SetupEvents();

    // 2) Create game object
    Game game(settings);
    game.on_event_hook = [&](const Event& event) {
        if (event.IsQuitEvent())
        {
            game.Stop();
            return true;
        }
        else if (event.IsKeyboardEvent())
        {
            const auto& args = event.GetKeyboardEventArgs();
            if (args.Key() == KeyCode::ESCAPE)
            {
                game.Stop();
                return true;
            }
        }
        else if (event.IsCustomEvent())
        {
            return perch::ProcessCustomEvent(event.GetCustomEventArgs());
        }

        return false;
    };

    try
    {
        PackFile pack("res/assets.pack");
        g_game.game = &game;
        g_game.pack = &pack;
        g_game.ratios = game_ratios(16.f, 2.f, ((game.window->IsHighDPI()) ? 2.f : 1.f));

        perch::PushScene(chrono_scene_overworld);
        game.Run();

        perch::DestroyAllScenes();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
    }

    return 0;
}
