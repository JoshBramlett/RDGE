#include <rdge/gameobjects/game.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/timer.hpp>

#ifdef RDGE_DEBUG
#include <rdge/debug.hpp>
#include <rdge/debug/widgets.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_rdge.h>
#endif

#include <SDL_assert.h>

namespace rdge {

Game::Game (const app_settings& s)
    : settings(s)
{
    SDL_assert(this->settings.target_fps >= 30);

    ILOG() << "Constructing Game object";
    this->window = std::make_unique<Window>(this->settings);
}

Game::~Game (void) noexcept
{
    DLOG() << "Destroying Game object";
}

// The game object provides a guarantee to the scenes that game loop events
// will not be sent after the request to terminate or while it's in a period of
// hibernation.  In order to facilitate this request calls to Hibernate and
// Terminate are deferred to the end of the game loop.  This prevents a
// potential scenario where the scene would be sent a game loop event after it
// has already processed it's cleanup code.

void
Game::PushScene (std::shared_ptr<IScene> scene)
{
    SDL_assert(!m_pushDeferred && !m_popDeferred);

    if (!m_running)
    {
        if (!m_sceneStack.empty())
        {
            auto& current_scene = m_sceneStack.back();
            current_scene->Hibernate();
        }
    }
    else
    {
        // Defer current_scene hibernation till end of game loop
        m_pushDeferred = true;
        m_sceneStack.push_back(scene);
    }

    scene->Initialize();
    m_sceneStack.push_back(scene);
}

void
Game::PopScene (void)
{
    SDL_assert(m_running);
    SDL_assert(!m_sceneStack.empty());
    SDL_assert(!m_pushDeferred && !m_popDeferred);

    // Defer current_scene termination till end of game loop
    m_popDeferred = true;
    m_sceneStack.pop_back();

    if (!m_sceneStack.empty())
    {
        auto& new_current_scene = m_sceneStack.back();
        new_current_scene->Activate();
    }
}

void
Game::Run (void)
{
    Event event;
    Timer timer;

    bool using_vsync = this->window->IsUsingVSYNC();
    uint32 frame_cap = 1000 / this->settings.target_fps;

#ifdef RDGE_DEBUG
    debug::InitializeRenderer();
    bool display_debug_overlay = false;
#endif

    m_running = true;
    timer.Start();
    while (m_running)
    {
        uint32 frame_start = timer.Ticks();
        if (m_sceneStack.empty())
        {
            m_running = false;
            break;
        }

        // ref count must be increased b/c scene could be removed from the collection
        // during the loop execution
        auto current_scene = m_sceneStack.back();
        while (PollEvent(&event))
        {
#ifdef RDGE_DEBUG
            ImGui_ImplRDGE_ProcessEvent(&event.sdl_event);

            if (event.IsKeyboardEvent())
            {
                const auto& args = event.GetKeyboardEventArgs();
                if (args.PhysicalKey() == ScanCode::F1 && args.IsKeyPressed())
                {
                    display_debug_overlay = !display_debug_overlay;
                }
            }

            ImGuiIO& io = ImGui::GetIO();
            if ((io.WantCaptureKeyboard && event.IsKeyboardEvent()) ||
                (io.WantCaptureMouse && event.IsMouseEvent()) ||
                (io.WantTextInput && event.IsTextInputEvent()))
            {
                // Suppress events if ImGui wants them
                continue;
            }
#endif

            if (!(this->on_event_hook && this->on_event_hook(event)))
            {
                current_scene->OnEvent(event);
            }
        }

#ifdef RDGE_DEBUG
        ImGui_ImplRDGE_NewFrame(static_cast<SDL_Window*>(*this->window.get()));

        if (display_debug_overlay)
        {
            // Overlay: Frame rate
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
            ImGui::SetNextWindowSize(ImVec2(static_cast<float>(io.DisplaySize.x),
                                            static_cast<float>(io.DisplaySize.y)));

            const auto overlay_flags = ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoInputs |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoScrollbar;
            ImGui::Begin("Overlay", nullptr, ImVec2(0.f, 0.f), 0.f, overlay_flags);

            ImGui::SetCursorPos(ImVec2(10.f, 30.f));
            ImGui::Text("Memory Tracker: %s", (debug::MEMORY_TRACKING_ENABLED ? "Enabled" : "Disabled"));
            ImGui::Text("%.3f frames/sec", io.Framerate);
            ImGui::End();

            // Main menu
            static debug::scene_widget_settings scene_widgets;
            static bool imgui_show_test_window = false;
            static bool imgui_show_memory_tracker = false;
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Scene"))
                {
                    ImGui::MenuItem("Camera", nullptr, &scene_widgets.show_camera_widget);
                    ImGui::MenuItem("Physics", nullptr, &scene_widgets.show_physics_widget);
                    ImGui::MenuItem("Graphics", nullptr, &scene_widgets.show_graphics_widget);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Memory"))
                {
                    ImGui::MenuItem("Tracker", nullptr, &imgui_show_memory_tracker, debug::MEMORY_TRACKING_ENABLED);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("ImGui"))
                {
                    ImGui::MenuItem("Show Test Window", nullptr, &imgui_show_test_window);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            current_scene->Debug_OnWidgetUpdate(scene_widgets);

            if (imgui_show_test_window)
            {
                ImGui::ShowTestWindow();
            }

            if (imgui_show_memory_tracker)
            {
                debug::ShowMemoryTracker(&imgui_show_memory_tracker);
            }
        }
#endif

        delta_time dt(timer.TickDelta());
        if (!(this->on_update_hook && this->on_update_hook(dt)))
        {
            current_scene->OnUpdate(dt);
        }

        this->window->Clear();
        if (!(this->on_render_hook && this->on_render_hook()))
        {
            current_scene->OnRender();
            debug::FlushRenderer();
        }

#ifdef RDGE_DEBUG
        ImGui::Render();
#endif

        this->window->Present();

        if (m_pushDeferred)
        {
            current_scene->Hibernate();
        }
        else if (m_popDeferred)
        {
            current_scene->Terminate();
        }

        if (!using_vsync)
        {
            uint32 frame_length = timer.Ticks() - frame_start;
            if (frame_length < frame_cap)
            {
                SDL_Delay(frame_cap - frame_length);
            }
        }
    }
}

void
Game::Stop (void)
{
    m_running = false;
}

} // namespace rdge
