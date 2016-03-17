#pragma once

#include <rdge/types.hpp>
#include <rdge/viewport.hpp>
#include <rdge/random.hpp>
#include <rdge/gameobjects/ientity.hpp>
#include <rdge/gameobjects/icollidable.hpp>

namespace pong {

using namespace RDGE::GameObjects;

class Ball final : public IEntity, public ICollidable
{
public:
    explicit Ball (void);

    virtual ~Ball (void) { }

    void HandleEvents (const SDL_Event&) { }

    void Update (RDGE::UInt32 ticks);

    void Render (const RDGE::Window& window);

    std::string Tag (void) const
    {
        return "Ball";
    }

    RDGE::Graphics::Rect CollisionAABB (void) const;

    void BounceX (RDGE::Int32 x_coordinate);

    void BounceY (RDGE::Int32 y_coordinate);

    void ResetPosition (void);

private:
    RDGE::Graphics::Rect m_ball;

    double m_locationX;
    double m_locationY;
    double m_velocityMultiplierX;
    double m_velocityMultiplierY;

    RDGE::Random m_random;
    RDGE::Int32  m_ballSize;
};

} // namespace pong
