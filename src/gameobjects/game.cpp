#include <rdge/gameobjects/game.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

namespace {
    constexpr RDGE::UInt32 MIN_FRAME_RATE = 30;
}

Game::Game (const game_settings& settings)
    : m_settings(settings)
    , m_window(nullptr)
    , m_running(false)
    , m_currentScene(nullptr)
{
    DLOG("Constructing Game object");

    if (m_settings.target_fps < MIN_FRAME_RATE)
    {
        m_settings.target_fps = MIN_FRAME_RATE;
    }

    m_window = std::make_unique<RDGE::GLWindow>(
                                                m_settings.window_title,
                                                m_settings.target_width,
                                                m_settings.target_height,
                                                m_settings.fullscreen,
                                                false, /* resizable */
                                                m_settings.use_vsync
                                               );
}

Game::~Game (void)
{
    DLOG("Destroying Game object");
}

std::shared_ptr<Scene>
Game::CurrentScene (void) const noexcept
{
    if (m_sceneStack.empty() == false)
    {
        return m_sceneStack.back();
    }

    return nullptr;
}

void
Game::PushScene (std::shared_ptr<Scene> scene)
{
    if (m_sceneStack.empty() == false)
    {
        auto current_scene = m_sceneStack.back();
        current_scene->Pause();
    }

    scene->Initialize();
    m_sceneStack.emplace_back(scene);
}

void
Game::PopScene (void)
{
    if (m_sceneStack.empty())
    {
        RDGE_THROW("Called PopScene with an empty stack");
    }
    else
    {
        auto current_scene = m_sceneStack.back();
        current_scene->Terminate();
    }

    m_sceneStack.pop_back();

    if (m_sceneStack.empty() == false)
    {
        auto current_scene = m_sceneStack.back();
        current_scene->Resume();
    }
}

void
Game::Run (void)
{
    SDL_Event event;
    RDGE::Util::Timer timer;

    RDGE::UInt32 frame_cap = 1000 / m_settings.target_fps;

    m_running = true;
    timer.Start();
    while (m_running)
    {
        RDGE::UInt32 frame_start = timer.Ticks();
        m_currentScene = CurrentScene();
        if (!m_currentScene)
        {
            m_running = false;
            break;
        }

        while (SDL_PollEvent(&event))
        {
            ProcessEventPhase(event);
        }

        RDGE::UInt32 ticks = timer.TickDelta();
        ProcessUpdatePhase(ticks);

        m_window->Clear();
        ProcessRenderPhase();
        m_window->Present();

        // TODO: Detect if vsync is enabled.  This code should execute if either vsync is off
        //       or not enabled for that system
        if (m_settings.use_vsync == false)
        {
            RDGE::UInt32 frame_length = timer.Ticks() - frame_start;
            if (frame_length < frame_cap)
            {
                SDL_Delay(frame_cap - frame_length);
            }
        }
    }
}

void
Game::ProcessEventPhase (const SDL_Event& event)
{
    m_currentScene->ProcessEventPhase(event);
}

void
Game::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    m_currentScene->ProcessUpdatePhase(ticks);
}

void
Game::ProcessRenderPhase (void)
{
    m_currentScene->ProcessRenderPhase();
}

} // namespace GameObjects
} // namespace RDGE
