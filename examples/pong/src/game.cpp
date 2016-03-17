#include "game.hpp"

#include <rdge/application.hpp>
#include <rdge/graphics/rect.hpp>
#include <rdge/font.hpp>

#include <functional>
#include <iostream>

namespace pong {

using namespace RDGE::GameObjects;

PongGame::PongGame (const GameSettings& settings)
    : Game(settings)
    , m_state(GameState::Intro)
    , m_showFPS(true)
{
    using namespace std::placeholders;

    auto font_path = RDGE::Application::BasePath() + "res/fonts/OpenSansPX.ttf";
    m_font = std::make_shared<RDGE::Font>(font_path, 18);

    // create scene instances
    m_introScene = std::make_shared<IntroScene>(m_window);
    m_menuScene = std::make_shared<MenuScene>(m_window);
    m_settingsScene = std::make_shared<SettingsScene>(m_window);
    m_pongScene = std::make_shared<PongScene>(m_window);

    // setup event handlers
    auto fn_term = std::bind(&PongGame::OnSceneRequestPop, this, _1, _2);
    auto fn_push = std::bind(&PongGame::OnSceneRequestPush, this, _1, _2);
    m_introScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    m_menuScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    m_menuScene->RegisterEventHandler(SceneEventType::RequestingPush, fn_push);
    m_settingsScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    m_pongScene->RegisterEventHandler(SceneEventType::RequestingPush, fn_push);

    PushScene(m_introScene);
}

void
PongGame::ProcessEvent (const SDL_Event& event)
{
    if (event.type == SDL_QUIT)
    {
        m_running = false;
    }
    else if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_f:
                m_showFPS = !m_showFPS;
                break;
        }
    }

    Game::ProcessEvent(event);
}

void
PongGame::ProcessRender (const RDGE::Window& window)
{
    Game::ProcessRender(window);

    // make FPS topmost - render after all game objects
    if (m_showFPS)
    {
        auto fps = "FPS:  " + std::to_string(static_cast<RDGE::UInt32>(window.FrameRate()));
        auto surface = m_font->RenderUTF8(fps, RDGE::Color::White());

        RDGE::Texture texture { window.Renderer(), surface.RawPtr() };
        window.Draw(texture, RDGE::Graphics::Point(20, 20));
    }
}

void
PongGame::OnSceneRequestPop (
                             const RDGE::GameObjects::Scene*,
                             const RDGE::GameObjects::SceneEventArgs& args
                            )
{
    std::cout << "PongGame::OnSceneRequestPop "
              << "event.id=" << args.id << std::endl;

    if (m_state == GameState::Intro)
    {
        PopScene();
        PushScene(m_menuScene);

        m_state = GameState::MainMenu;
    }
    else if (m_state == GameState::MainMenu)
    {
        PopScene();
    }
}

void
PongGame::OnSceneRequestPush (
                              const RDGE::GameObjects::Scene*,
                              const RDGE::GameObjects::SceneEventArgs& args
                             )
{
    std::cout << "PongGame::OnSceneRequestPush "
              << "event.id=" << args.id << std::endl;

    if (m_state == GameState::MainMenu)
    {
        if (args.id == "new_game")
        {
            PopScene();
            PushScene(m_pongScene);

            m_state = GameState::Running;
        }
        else if (args.id == "settings")
        {
            PushScene(m_settingsScene);
        }
    }
    else if (m_state == GameState::MainMenu)
    {
        if (args.id == "pause")
        {

        }
    }
}

} // namespace pong
