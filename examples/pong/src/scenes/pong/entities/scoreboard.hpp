#pragma once

#include <rdge/types.hpp>
#include <rdge/font.hpp>
#include <rdge/gameobjects/ientity.hpp>

namespace pong {

using namespace RDGE::GameObjects;

class Scoreboard final : public IEntity
{
public:
    explicit Scoreboard (void);

    virtual ~Scoreboard (void) { }

    void HandleEvents (const SDL_Event&) { }

    void Update (RDGE::UInt32) { }

    void Render (const RDGE::Window& window);

    std::string Tag (void) const
    {
        return "Scoreboard";
    }

    void SetPlayerScore (RDGE::UInt32 score)
    {
        m_playerScore = score;
    }

    void SetOpponentScore (RDGE::UInt32 score)
    {
        m_opponentScore = score;
    }

private:
    // TODO : Remove shared ptr, or implement asset manager
    std::shared_ptr<RDGE::Font> m_font;

    RDGE::UInt32 m_playerScore;
    RDGE::UInt32 m_opponentScore;
};

} // namespace pong
