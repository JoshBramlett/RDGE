#pragma once

#include <rdge/types.hpp>
#include <rdge/font.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/game.hpp>
#include <rdge/gameobjects/scene.hpp>

#include "scenes/intro/introscene.hpp"
#include "scenes/menu/menuscene.hpp"
#include "scenes/settings/settingsscene.hpp"
#include "scenes/pong/pongscene.hpp"

#include <string>
#include <memory>

namespace pong {

class PongGame final : public RDGE::GameObjects::Game
{
public:
    enum class GameState : RDGE::UInt8
    {
        Intro = 1,
        MainMenu,
        Running,
        Paused
    };

    explicit PongGame (const RDGE::GameObjects::GameSettings& settings);

    virtual ~PongGame (void) { }

protected:

    virtual void ProcessEvent (const SDL_Event& e) override;

    virtual void ProcessRender (const RDGE::Window& window) override;

private:
    void OnSceneRequestPop (
                            const RDGE::GameObjects::Scene*,
                            const RDGE::GameObjects::SceneEventArgs&
                           );

    void OnSceneRequestPush (
                             const RDGE::GameObjects::Scene*,
                             const RDGE::GameObjects::SceneEventArgs&
                            );

    GameState                   m_state;
    std::shared_ptr<RDGE::Font> m_font;
    bool                        m_showFPS;

    std::shared_ptr<IntroScene>    m_introScene;
    std::shared_ptr<MenuScene>     m_menuScene;
    std::shared_ptr<SettingsScene> m_settingsScene;
    std::shared_ptr<PongScene>     m_pongScene;
};

} // namespace pong
