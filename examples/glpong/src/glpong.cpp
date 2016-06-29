#include "glpong.hpp"

#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/rect.hpp>
#include <rdge/util/logger.hpp>

#include <functional>
#include <iostream>

namespace glpong {

using namespace RDGE::Assets;
using namespace RDGE::GameObjects;
using LogLevel = RDGE::LogLevel;

GLPongGame::GLPongGame (const RDGE::game_settings& settings)
    : Game(settings)
    , m_font(nullptr)
    , m_showFPS(true)
{
    using namespace std::placeholders;

    RDGE::SetEventState(RDGE::EventType::FingerDown, false);
    RDGE::SetEventState(RDGE::EventType::FingerUp, false);
    RDGE::SetEventState(RDGE::EventType::FingerMotion, false);
    RDGE::SetEventState(RDGE::EventType::MultiGesture, false);

    //auto font_path = RDGE::Application::BasePath() + "res/fonts/OpenSansPX.ttf";
    //m_font = std::make_shared<RDGE::Font>(font_path, 18);

    // create scene instances
    //m_introScene = std::make_shared<IntroScene>(m_window.get());
    m_chronoScene = std::make_shared<ChronoScene>(m_window.get());
    //m_menuScene = std::make_shared<MenuScene>(m_window);
    //m_settingsScene = std::make_shared<SettingsScene>(m_window);
    //m_pongScene = std::make_shared<PongScene>(m_window);

    // setup event handlers
    //auto fn_term = std::bind(&GLPongGame::OnSceneRequestPop, this, _1, _2);
    //auto fn_push = std::bind(&PongGame::OnSceneRequestPush, this, _1, _2);
    //m_introScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    //m_menuScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    //m_menuScene->RegisterEventHandler(SceneEventType::RequestingPush, fn_push);
    //m_settingsScene->RegisterEventHandler(SceneEventType::RequestingPop, fn_term);
    //m_pongScene->RegisterEventHandler(SceneEventType::RequestingPush, fn_push);

    //PushScene(m_introScene);
    PushScene(m_chronoScene);
}

void
GLPongGame::ProcessEventPhase (RDGE::Event& event)
{
    if (event.IsQuitEvent())
    {
        m_running = false;
    }
    else if (event.Type() == RDGE::EventType::KeyDown)
    {
        auto args = event.GetKeyboardEventArgs();
        switch (args.Key())
        {
            case RDGE::KeyCode::F:
                m_showFPS = !m_showFPS;
                break;
            default:
                break;
        }
    }

    Game::ProcessEventPhase(event);
}

void
GLPongGame::ProcessRenderPhase (void)
{
    Game::ProcessRenderPhase();

    // make FPS topmost - render after all game objects
    if (m_showFPS)
    {
        //std::cout << "FPS:" << m_window->FrameRate() << std::endl;

        //auto fps = "FPS:  " + std::to_string(static_cast<RDGE::UInt32>(window.FrameRate()));
        //auto surface = m_font->RenderUTF8(fps, RDGE::Color::White());

        //RDGE::Texture texture { window.Renderer(), surface.RawPtr() };
        //window.Draw(texture, RDGE::Graphics::Point(20, 20));
    }
}

void
GLPongGame::OnSceneRequestPop (void*, const SceneEventArgs& args)
{
    //RDGE::WriteToConsole(LogLevel::Debug, "PongGame::OnSceneRequestPop event.id=" + args.id);

    if (args.id == "intro_end")
    {
        //PopScene();
    }

    //if (m_state == GameState::Intro)
    //{
        //PopScene();
        //PushScene(m_menuScene);

        //m_state = GameState::MainMenu;
    //}
    //else if (m_state == GameState::MainMenu)
    //{
        //PopScene();
    //}
}

void
GLPongGame::OnSceneRequestPush (void*, const SceneEventArgs& args)
{
    RDGE::WriteToConsole(LogLevel::Debug, "PongGame::OnSceneRequestPush event.id=" + args.id);

    //if (m_state == GameState::MainMenu)
    //{
        //if (args.id == "new_game")
        //{
            //PopScene();
            //PushScene(m_pongScene);

            //m_state = GameState::Running;
        //}
        //else if (args.id == "settings")
        //{
            //PushScene(m_settingsScene);
        //}
    //}
    //else if (m_state == GameState::MainMenu)
    //{
        //if (args.id == "pause")
        //{

        //}
    //}
}

} // namespace glpong
