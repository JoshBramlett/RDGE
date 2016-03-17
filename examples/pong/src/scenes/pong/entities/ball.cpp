#include "ball.hpp"

namespace pong {

namespace {

constexpr RDGE::Int32 BASE_VELOCITY = 1200;
constexpr double BASE_BALL_SIZE     = 0.0277778;  // 20px / 720

} // anonymous namespace

Ball::Ball (void)
{
    //auto dimensions = m_viewport.Dimensions();
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    m_ballSize = static_cast<RDGE::Int32>(dimensions.h * BASE_BALL_SIZE);

    m_ball = RDGE::Graphics::Rect(0, 0, m_ballSize, m_ballSize);

    ResetPosition();
}

void
Ball::Update (RDGE::UInt32 ticks)
{
    float delta = static_cast<float>(ticks) / 1000.0f;
    m_locationX += (BASE_VELOCITY * m_velocityMultiplierX) * delta;
    m_locationY += (BASE_VELOCITY * m_velocityMultiplierY) * delta;
}

void
Ball::Render (const RDGE::Window& window)
{
    m_ball.x = static_cast<RDGE::Int32>(m_locationX);
    m_ball.y = static_cast<RDGE::Int32>(m_locationY);
    window.DrawPrimitiveRect(&m_ball, RDGE::Color::White());
}

RDGE::Graphics::Rect
Ball::CollisionAABB (void) const
{
    return RDGE::Graphics::Rect(
                                static_cast<RDGE::Int32>(m_locationX),
                                static_cast<RDGE::Int32>(m_locationY),
                                m_ballSize,
                                m_ballSize
                               );
}

void
Ball::BounceX (RDGE::Int32 x_coordinate)
{
    m_velocityMultiplierX = -m_velocityMultiplierX;
    m_locationX = static_cast<double>(x_coordinate);
    m_ball.x = static_cast<RDGE::Int32>(m_locationX);
}

void
Ball::BounceY (RDGE::Int32 y_coordinate)
{
    m_velocityMultiplierY = -m_velocityMultiplierY;
    m_locationY = static_cast<double>(y_coordinate);
    m_ball.y = static_cast<RDGE::Int32>(m_locationY);
}

void
Ball::ResetPosition (void)
{
    // Randomly sets the x and y velocity which represents the direction.
    // Sets the value between .25 and .75 to avoid following too close to
    // the x and y axis.
    auto ratio = m_random.Next(25, 75) / 100.0;
    if (m_random.Next(1) == 0)
    {
        m_velocityMultiplierX = (m_random.Next(1) == 0) ? ratio : -ratio;
        m_velocityMultiplierY = (m_random.Next(1) == 0) ? (1.0 - ratio) : -(1 - ratio);
    }
    else
    {
        m_velocityMultiplierX = (m_random.Next(1) == 0) ? (1.0 - ratio) : -(1 - ratio);
        m_velocityMultiplierY = (m_random.Next(1) == 0) ? ratio : -ratio;
    }

    //auto dimensions = m_viewport.Dimensions();
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    m_locationX = dimensions.w / 2.0;
    m_locationY = dimensions.h / 2.0;
    m_ball.x = static_cast<RDGE::Int32>(m_locationX);
    m_ball.y = static_cast<RDGE::Int32>(m_locationY);
}

} // namespace pong
