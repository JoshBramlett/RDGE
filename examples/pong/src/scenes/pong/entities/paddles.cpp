#include "paddles.hpp"

namespace pong {

namespace {

constexpr RDGE::Int32 BASE_VELOCITY      = 400;

constexpr double BASE_PADDLE_WIDTH  = 0.0156250;     // 20px  / 1280
constexpr double BASE_PADDLE_HEIGHT = 0.2777778;     // 200px / 720
constexpr double BASE_BUMPER        = 0.0138889;     // 10px  / 720
constexpr double BASE_GOAL_MARGIN   = 0.0390625;     // 50px  / 1280

} // anonymous namespace

PlayerPaddle::PlayerPaddle (void)
    : m_velocityY(0)
{
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    //auto dimensions = m_viewport.Dimensions();
    auto paddle_width  = static_cast<RDGE::Int32>(dimensions.w * BASE_PADDLE_WIDTH);
    auto paddle_height = static_cast<RDGE::Int32>(dimensions.h * BASE_PADDLE_HEIGHT);
    auto goal_margin   = static_cast<RDGE::Int32>(dimensions.w * BASE_GOAL_MARGIN);

    m_minY = static_cast<RDGE::Int32>(dimensions.h * BASE_BUMPER);
    m_maxY = dimensions.h - paddle_height - m_minY;

    m_locationY = (dimensions.h / 2) - (paddle_height / 2) - m_minY;

    m_paddle = RDGE::Graphics::Rect(
                                    goal_margin,
                                    m_locationY,
                                    paddle_width,
                                    paddle_height
                                   );
}

void
PlayerPaddle::HandleEvents (const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            m_velocityY -= BASE_VELOCITY;
            break;
        case SDLK_DOWN:
            m_velocityY += BASE_VELOCITY;
            break;
        }
    }

    if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            m_velocityY += BASE_VELOCITY;
            break;
        case SDLK_DOWN:
            m_velocityY -= BASE_VELOCITY;
            break;
        }
    }
}

void
PlayerPaddle::Update (RDGE::UInt32 ticks)
{
    float delta = static_cast<float>(ticks) / 1000.0f;
    m_locationY += m_velocityY * delta;

    auto y = static_cast<RDGE::Int32>(m_locationY);
    if (y < m_minY)
    {
        m_locationY = static_cast<double>(m_minY);
    }
    else if (y > m_maxY)
    {
        m_locationY = static_cast<double>(m_maxY);
    }
}

void
PlayerPaddle::Render (const RDGE::Window& window)
{
    m_paddle.y = static_cast<RDGE::Int32>(m_locationY);
    window.DrawPrimitiveRect(&m_paddle, RDGE::Color::White());
}

OpponentPaddle::OpponentPaddle (void)
    : m_velocityY(0)
{
    //auto dimensions = m_viewport.Dimensions();
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    auto paddle_width  = static_cast<RDGE::Int32>(dimensions.w * BASE_PADDLE_WIDTH);
    auto paddle_height = static_cast<RDGE::Int32>(dimensions.h * BASE_PADDLE_HEIGHT);
    auto goal_margin   = static_cast<RDGE::Int32>(dimensions.w * BASE_GOAL_MARGIN);

    m_minY = static_cast<RDGE::Int32>(dimensions.h * BASE_BUMPER);
    m_maxY = dimensions.h - paddle_height - m_minY;

    m_locationY = (dimensions.h / 2) - (paddle_height / 2) - m_minY;

    m_paddle = RDGE::Graphics::Rect(
                                    dimensions.w - paddle_width - goal_margin,
                                    m_locationY,
                                    paddle_width,
                                    paddle_height
                                   );
}

void
OpponentPaddle::Update (RDGE::UInt32 ticks)
{
    float delta = static_cast<float>(ticks) / 1000.0f;
    if (m_paddle.Bottom() < m_ballCentroid.y)
    {
        m_velocityY = BASE_VELOCITY;
    }
    else if (m_paddle.Top() > m_ballCentroid.y)
    {
        m_velocityY = -BASE_VELOCITY;
    }
    else
    {
        m_velocityY = 0;
    }

    m_locationY += m_velocityY * delta;

    auto y = static_cast<RDGE::Int32>(m_locationY);
    if (y < m_minY)
    {
        m_locationY = static_cast<double>(m_minY);
    }
    else if (y > m_maxY)
    {
        m_locationY = static_cast<double>(m_maxY);
    }
}

void
OpponentPaddle::Render (const RDGE::Window& window)
{
    m_paddle.y = static_cast<RDGE::Int32>(m_locationY);
    window.DrawPrimitiveRect(&m_paddle, RDGE::Color::White());
}

void
OpponentPaddle::UpdateBallPosition (const RDGE::Graphics::Point& centroid)
{
    m_ballCentroid = centroid;
}

} // namespace pong
