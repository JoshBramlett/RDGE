#pragma once

#include <rdge/types.hpp>
#include <rdge/config.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/gameobjects/game.hpp>
#include <rdge/gameobjects/scene.hpp>

#include <SDL.h>

#include "scenes/introscene.hpp"

namespace glpong {

class GLPongGame : public RDGE::GameObjects::Game
{
public:
    explicit GLPongGame (const RDGE::game_settings& settings);

    virtual ~GLPongGame (void) { }

protected:
    virtual void ProcessEventPhase (const SDL_Event& e) override;

    virtual void ProcessRenderPhase (void) override;

private:
    void OnSceneRequestPop (void* sender, const RDGE::GameObjects::SceneEventArgs& args);

    void OnSceneRequestPush (void* sender, const RDGE::GameObjects::SceneEventArgs& args);

    std::shared_ptr<RDGE::Assets::Font> m_font;
    bool                                m_showFPS;

    std::shared_ptr<IntroScene>    m_introScene;
    //std::shared_ptr<MenuScene>     m_menuScene;
    //std::shared_ptr<SettingsScene> m_settingsScene;
    //std::shared_ptr<PongScene>     m_pongScene;

};

} // namespace glpong
