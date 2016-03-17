#pragma once

#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/scene.hpp>

namespace pong {

// IntroScene is the game's starting point.  It simply displays
// our vanity screen when the application loads
class IntroScene final : public RDGE::GameObjects::Scene
{
public:
    explicit IntroScene (const RDGE::Window& window);

    virtual ~IntroScene (void) { }

    void Update (RDGE::UInt32 ticks) override;

private:
    // Length (in milliseconds) the scene will be present
    static constexpr RDGE::UInt32 SCENE_LENGTH = 2000;

    RDGE::Int32 m_duration;
};

} // namespace pong
