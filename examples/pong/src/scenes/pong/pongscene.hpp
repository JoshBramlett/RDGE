#pragma once

#include <rdge/types.hpp>
#include <rdge/cursor.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/scene.hpp>

#include <SDL.h>

namespace pong {

class PongScene final : public RDGE::GameObjects::Scene
{
public:
    explicit PongScene (const RDGE::Window& window);

    virtual ~PongScene (void) { }

    virtual void HandleEvents (const SDL_Event& e) override;

    virtual void Update (RDGE::UInt32 ticks) override;

    virtual void Initialize (void) override;

    virtual void Terminate (void) override;

    virtual void Pause (void) override;

    virtual void Resume (void) override;

private:
    RDGE::UInt32 m_playerScore;
    RDGE::UInt32 m_opponentScore;
    RDGE::Cursor m_cursor;
};


} // namespace pong
