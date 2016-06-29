#include "gui.hpp"

#include <rdge/application.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/rect.hpp>
#include <rdge/util/logger.hpp>

#include <functional>
#include <iostream>

namespace gui {

using namespace RDGE::GameObjects;

GUIGame::GUIGame (const RDGE::game_settings& settings)
    : Game(settings)
{
    // disable unwanted events
    RDGE::SetEventState(RDGE::EventType::FingerDown, false);
    RDGE::SetEventState(RDGE::EventType::FingerUp, false);
    RDGE::SetEventState(RDGE::EventType::FingerMotion, false);
    RDGE::SetEventState(RDGE::EventType::MultiGesture, false);

    // create scene instances
    m_guiScene = std::make_shared<GUIScene>(m_window.get());
    PushScene(m_guiScene);
}

void
GUIGame::ProcessEventPhase (RDGE::Event& event)
{
    if (event.IsQuitEvent())
    {
        m_running = false;
    }
    else if (event.Type() == RDGE::EventType::KeyUp)
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.Key() == RDGE::KeyCode::Escape)
        {
            m_running = false;
        }
    }

    Game::ProcessEventPhase(event);
}

} // namespace gui
