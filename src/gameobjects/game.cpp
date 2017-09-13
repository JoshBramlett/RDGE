#include <rdge/gameobjects/game.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/debug/renderer.hpp>

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
    SDL_assert((m_flags & ANY_DEFERRED) == 0);

    if (m_flags & RUNNING)
    {
        // Defer current_scene hibernation till end of game loop
        m_flags |= PUSH_DEFERRED;
    }
    else
    {
        if (!m_sceneStack.empty())
        {
            auto& current_scene = m_sceneStack.back();
            if (current_scene == scene)
            {
                DLOG() << "Push scene ignored.  Scene already current";
                return;
            }

            current_scene->Hibernate();
        }

        scene->Initialize();
    }

    m_sceneStack.emplace_back(std::move(scene));
}

void
Game::PopScene (void)
{
    SDL_assert(m_flags & RUNNING);
    SDL_assert((m_flags & ANY_DEFERRED) == 0);
    SDL_assert(!m_sceneStack.empty());

    // Defer current_scene termination till end of game loop
    m_flags |= POP_DEFERRED;
    m_sceneStack.pop_back();
}

void
Game::SwapScene (std::shared_ptr<IScene> scene)
{
    SDL_assert(m_flags & RUNNING);
    SDL_assert((m_flags & ANY_DEFERRED) == 0);
    SDL_assert(!m_sceneStack.empty());

    if (m_sceneStack.back() == scene)
    {
        DLOG() << "Swap scene ignored.  Scene already current";
        return;
    }

    // Defer current_scene termination till end of game loop
    m_flags |= SWAP_DEFERRED;
    m_sceneStack.pop_back();

    m_sceneStack.emplace_back(std::move(scene));
}

void
Game::Run (void)
{
    Event event;
    Timer timer;

    bool using_vsync = this->window->IsUsingVSYNC();
    uint32 frame_cap = 1000 / this->settings.target_fps;

    debug::InitializeOverlay();

    m_flags |= RUNNING;
    timer.Start();
    while (m_flags & RUNNING)
    {
        uint32 frame_start = timer.Ticks();
        if (m_sceneStack.empty())
        {
            m_flags &= ~RUNNING;
            break;
        }

        // ref count must be increased b/c scene could be removed from the collection
        // during the loop execution
        auto current_scene = m_sceneStack.back();
        while (PollEvent(&event))
        {
            if (debug::ProcessOnEvent(event))
            {
                // debug UI requesting events - suppress from scene
                continue;
            }

            if (!(this->on_event_hook && this->on_event_hook(event)))
            {
                current_scene->OnEvent(event);
            }
        }

        delta_time dt(timer.TickDelta());
        if (!(this->on_update_hook && this->on_update_hook(dt)))
        {
            current_scene->OnUpdate(dt);
        }

        debug::ProcessOnUpdate(static_cast<SDL_Window*>(*this->window.get()), dt);

        this->window->Clear();
        if (!(this->on_render_hook && this->on_render_hook()))
        {
            current_scene->OnRender();
        }

        debug::ProcessOnRender();
        this->window->Present();

        if (m_flags & PUSH_DEFERRED)
        {
            current_scene->Hibernate();
            m_flags &= ~PUSH_DEFERRED;

            auto& new_current_scene = m_sceneStack.back();
            new_current_scene->Initialize();
        }
        else if (m_flags & POP_DEFERRED)
        {
            current_scene->Terminate();
            m_flags &= ~POP_DEFERRED;

            auto& new_current_scene = m_sceneStack.back();
            new_current_scene->Activate();
        }
        else if (m_flags & SWAP_DEFERRED)
        {
            current_scene->Terminate();
            m_flags &= ~SWAP_DEFERRED;

            auto& new_current_scene = m_sceneStack.back();
            new_current_scene->Initialize();
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
    m_flags &= ~RUNNING;
}

} // namespace rdge
