#include "scoreboard.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/texture.hpp>
#include <rdge/graphics/point.hpp>

#include <sstream>
#include <iomanip>
#include <memory>

namespace pong {

namespace {

constexpr RDGE::Int32 FIXED_Y_LOCATION = 10;
constexpr RDGE::Int32 CENTER_OFFSET    = 40;

} // anonymous namespace

Scoreboard::Scoreboard (void)
    : m_playerScore(0)
    , m_opponentScore(0)
{
    auto path = RDGE::Application::BasePath() + "res/fonts/OpenSansPX.ttf";
    m_font = std::make_shared<RDGE::Font>(path, 96);
}

void
Scoreboard::Render (const RDGE::Window& window)
{
    std::stringstream player_score;
    player_score << std::setfill('0')
                 << std::setw(2)
                 << m_playerScore;

    std::stringstream opponent_score;
    opponent_score << std::setfill('0')
                   << std::setw(2)
                   << m_opponentScore;

    auto psurface = m_font->RenderUTF8(player_score.str(), RDGE::Color::White());
    RDGE::Texture ptexture(window.Renderer(), psurface.RawPtr());

    auto osurface = m_font->RenderUTF8(opponent_score.str(), RDGE::Color::White());
    RDGE::Texture otexture(window.Renderer(), osurface.RawPtr());

    auto mid_width = window.Width() / 2;
    RDGE::Int32 player_xpos = mid_width - CENTER_OFFSET - ptexture.Width();
    RDGE::Int32 opponent_xpos = mid_width + CENTER_OFFSET;

    auto plocation = RDGE::Graphics::Point(player_xpos, FIXED_Y_LOCATION);
    auto olocation = RDGE::Graphics::Point(opponent_xpos, FIXED_Y_LOCATION);
    window.Draw(ptexture, plocation);
    window.Draw(otexture, olocation);
}

} // namespace pong
