#include <rdge/gameobjects/game.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/internal/exception_macros.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

namespace {
    constexpr RDGE::UInt32 MinimumFrameRate = 24;
}

Game::Game (const GameSettings& settings)
    : m_settings(settings)
    , m_running(false)
    , m_currentScene(nullptr)
{
    if (m_settings.target_fps < MinimumFrameRate)
    {
        m_settings.target_fps = MinimumFrameRate;
    }

    m_window = RDGE::Window(
                            settings.window_title,
                            settings.target_width,
                            settings.target_height,
                            settings.use_vsync
                           );
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
            ProcessEvent(event);
        }

        RDGE::UInt32 ticks = timer.TickDelta();
        ProcessUpdate(ticks);

        m_window.Clear();
        ProcessRender(m_window);
        m_window.Present();

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
Game::ProcessEvent (const SDL_Event& event)
{
    m_currentScene->HandleEvents(event);
}

void
Game::ProcessUpdate (RDGE::UInt32 ticks)
{
    m_currentScene->Update(ticks);
}

void
Game::ProcessRender (const RDGE::Window& window)
{
    m_currentScene->Render(window);
}

} // namespace GameObjects
} // namespace RDGE
