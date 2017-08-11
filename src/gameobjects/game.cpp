#include <rdge/gameobjects/game.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/debug.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_rdge.h>

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

    debug::InitializeRenderer();

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
            ImGui_ImplRDGE_ProcessEvent(&event.sdl_event);
            if (!(this->on_event_hook && this->on_event_hook(event)))
            {
                current_scene->OnEvent(event);
            }
        }

        ImGui_ImplRDGE_NewFrame(static_cast<SDL_Window*>(*this->window.get()));
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    //1000.0f / ImGui::GetIO().Framerate,
                    //ImGui::GetIO().Framerate);

        //bool show_another_window = true;
        //ImGui::Begin("Another Window", &show_another_window);
        //ImGui::Text("Hello");
        //ImGui::End();

        ImGui::ShowTestWindow();

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

        ImGui::Render();
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
