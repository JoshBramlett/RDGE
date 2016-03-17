#include "pongscene.hpp"
#include "entities/net.hpp"
#include "entities/scoreboard.hpp"
#include "entities/paddles.hpp"
#include "entities/ball.hpp"

#include <rdge/graphics/rect.hpp>

#include <memory>

namespace pong {

PongScene::PongScene (const RDGE::Window& window)
    : RDGE::GameObjects::Scene(window)
    , m_playerScore(0)
    , m_opponentScore(0)
{
    RDGE::Unused(window);
    AddEntity("net", std::make_shared<Net>());
    AddEntity("scoreboard", std::make_shared<Scoreboard>());
    AddEntity("player", std::make_shared<PlayerPaddle>());
    AddEntity("opponent", std::make_shared<OpponentPaddle>());
    AddEntity("ball", std::make_shared<Ball>());
}

void
PongScene::Initialize (void)
{
    m_playerScore = 0;
    m_opponentScore = 0;

    auto scoreboard = std::dynamic_pointer_cast<Scoreboard>(GetEntity("scoreboard"));
    auto player = std::dynamic_pointer_cast<PlayerPaddle>(GetEntity("player"));
    auto opponent = std::dynamic_pointer_cast<OpponentPaddle>(GetEntity("opponent"));
    auto ball = std::dynamic_pointer_cast<Ball>(GetEntity("ball"));


    /////////////////////////////////////////////////
    //
    // TODO:  Entities need a Initialize() method
    //
    /////////////////////////////////////////////////


    scoreboard->SetPlayerScore(0);
    scoreboard->SetOpponentScore(0);
    ball->ResetPosition();

    m_cursor.Hide();
}

void
PongScene::Terminate (void)
{
    m_cursor.Show();
}

void
PongScene::Pause (void)
{
    m_cursor.Show();
}

void
PongScene::Resume (void)
{
    m_cursor.Hide();
}

void PongScene::HandleEvents (const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == SDLK_ESCAPE)
        {
            TriggerEvent(
                         SceneEventType::RequestingPush,
                         { "pause", SceneEventType::RequestingPush }
                        );
        }
    }

    Scene::HandleEvents(e);
}

void
PongScene::Update (RDGE::UInt32 ticks)
{
    RDGE::GameObjects::Scene::Update(ticks);

    auto scoreboard = std::dynamic_pointer_cast<Scoreboard>(GetEntity("scoreboard"));
    auto player = std::dynamic_pointer_cast<PlayerPaddle>(GetEntity("player"));
    auto opponent = std::dynamic_pointer_cast<OpponentPaddle>(GetEntity("opponent"));
    auto ball = std::dynamic_pointer_cast<Ball>(GetEntity("ball"));

    auto ball_rect = ball->CollisionAABB();
    opponent->UpdateBallPosition(ball_rect.Centroid());

    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    if (ball_rect.Left() <= dimensions.Left())
    {
        // cpu scores
        scoreboard->SetOpponentScore(++m_opponentScore);
        ball->ResetPosition();
    }

    if (ball_rect.Right() >= dimensions.Right())
    {
        // player scores
        scoreboard->SetPlayerScore(++m_playerScore);
        ball->ResetPosition();
    }

    if (ball_rect.Top() <= dimensions.Top())
    {
        ball->BounceY(dimensions.Top());
    }

    if (ball_rect.Bottom() >= dimensions.Bottom())
    {
        ball->BounceY(dimensions.Bottom() - ball_rect.h);
    }

    auto player_rect = player->CollisionAABB();
    if (player_rect.IntersectsWith(ball_rect))
    {
        ball->BounceX(player_rect.Right());
    }

    auto opponent_rect = opponent->CollisionAABB();
    if (opponent_rect.IntersectsWith(ball_rect))
    {
        ball->BounceX(opponent_rect.Left() - ball_rect.w);
    }
}

} // namespace pong
