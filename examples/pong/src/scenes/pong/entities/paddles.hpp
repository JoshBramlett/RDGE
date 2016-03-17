#pragma once

#include <rdge/types.hpp>
#include <rdge/viewport.hpp>
#include <rdge/gameobjects/ientity.hpp>
#include <rdge/gameobjects/icollidable.hpp>

namespace pong {

using namespace RDGE::GameObjects;

class PlayerPaddle final : public IEntity, public ICollidable
{
public:
    explicit PlayerPaddle (void);

    virtual ~PlayerPaddle (void) { }

    void HandleEvents (const SDL_Event& e) override;

    void Update (RDGE::UInt32 ticks) override;

    void Render (const RDGE::Window& window) override;

    std::string Tag (void) const override
    {
        return "Paddle";
    }

    RDGE::Graphics::Rect CollisionAABB (void) const override
    {
        return m_paddle;
    }

private:
    RDGE::Graphics::Rect m_paddle;

    double m_locationY;
    double m_velocityY;

    RDGE::Int32 m_minY;
    RDGE::Int32 m_maxY;
};

class OpponentPaddle final : public IEntity, public ICollidable
{
public:
    explicit OpponentPaddle (void);

    virtual ~OpponentPaddle (void) { }

    void HandleEvents (const SDL_Event&) override { }

    void Update (RDGE::UInt32 ticks) override;

    void Render (const RDGE::Window& window) override;

    std::string Tag (void) const override
    {
        return "Paddle";
    }

    RDGE::Graphics::Rect CollisionAABB (void) const override
    {
        return m_paddle;
    }

    void UpdateBallPosition (const RDGE::Graphics::Point& centroid);

private:
    RDGE::Graphics::Rect m_paddle;

    float m_locationY;
    float m_velocityY;

    RDGE::Int32 m_minY;
    RDGE::Int32 m_maxY;

    RDGE::Graphics::Point m_ballCentroid;
};

} // namespace pong
