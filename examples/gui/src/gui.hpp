#pragma once

#include <rdge/types.hpp>
#include <rdge/config.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/game.hpp>
#include <rdge/gameobjects/scene.hpp>

#include "scenes/guiscene.hpp"

namespace gui {

class GUIGame : public RDGE::GameObjects::Game
{
public:
    explicit GUIGame (const RDGE::game_settings& settings);

    virtual ~GUIGame (void) { }

protected:
    virtual void ProcessEventPhase (RDGE::Event& e) override;

private:
    std::shared_ptr<GUIScene> m_guiScene;
};

} // namespace gui
