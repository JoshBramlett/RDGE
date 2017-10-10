#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/assets.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/math.hpp>
#include <rdge/system.hpp>
#include <rdge/debug.hpp>

#include <imgui/imgui.h>

#include "scenes/test.hpp"
#include "scenes/tiles.hpp"
#include "scenes/tumbler.hpp"

#include <memory>

using namespace rdge;

class SceneSwapWidget : public rdge::debug::IWidget
{
public:
    SceneSwapWidget (const app_settings& settings)
        : game(settings)
    { }

    ~SceneSwapWidget (void) noexcept = default;

    void Update (void) override
    {
        if (!this->show)
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(210.f, 410.f), ImGuiSetCond_FirstUseEver);
        if (!ImGui::Begin("Scene", &this->show))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("Simulations:");
        if (ImGui::Combo("##Test",
						 &this->selected_index,
						 [](void* p, int index, const char** name) {
                            auto& scenes = *static_cast<SceneMap*>(p);
                            *name = scenes[index].first.c_str();
                            return true;
                         },
                         static_cast<void*>(&scenes),
                         scenes.size(),
                         scenes.size()))
        {
            if (this->selected_index != this->scene_index)
            {
                this->scene_index = this->selected_index;

                std::cout << scenes[scene_index].first << std::endl;
                game.SwapScene(scenes[scene_index].second);
            }
        }

        ImGui::Separator();
        ImGui::End();
    }

    void OnCustomRender (void) override { };

    void AddScene (std::string name, std::shared_ptr<rdge::IScene> scene)
    {
        scenes.emplace_back(std::make_pair(name, scene));
    }

    bool show = true;
	int scene_index = 0;
	int selected_index = 0;

    Game game;

    using SceneMap = std::vector<std::pair<std::string, std::shared_ptr<rdge::IScene>>>;
    SceneMap scenes;
};

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
    SceneSwapWidget widget(settings);
    widget.AddScene("Test", std::make_shared<TestScene>());
    widget.AddScene("Tiles", std::make_shared<TilesScene>());
    widget.AddScene("Tumbler", std::make_shared<TumblerScene>());

    debug::AddWidget(&widget);
    debug::settings::show_overlay = true;
    debug::settings::draw_physics_fixtures = true;

    widget.game.on_event_hook = [&](const Event& event) {
        if (event.IsQuitEvent())
        {
            widget.game.Stop();
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
                widget.game.Stop();
            }
        }

        return false;
    };

    widget.game.PushScene(widget.scenes[0].second);
    widget.game.Run();

    return 0;
}
