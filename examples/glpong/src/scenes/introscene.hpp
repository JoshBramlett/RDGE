#pragma once

#include <rdge/types.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/gameobjects/scene.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/layers/layer2d.hpp>

#include <memory>

namespace glpong {

// IntroScene is the game's starting point.  It simply displays
// our vanity screen when the application loads
class IntroScene final : public RDGE::GameObjects::Scene
{
public:
    explicit IntroScene (RDGE::GLWindow* window);

    virtual ~IntroScene (void) { }

    virtual void ProcessUpdatePhase (RDGE::UInt32 ticks) override;

private:
    // Length (in milliseconds) the scene will be present
    static constexpr RDGE::UInt32 SCENE_LENGTH = 2000;

    std::shared_ptr<RDGE::Graphics::Layer2D> m_layer;
    std::shared_ptr<RDGE::Graphics::Text>    m_msg;
    RDGE::Int32 m_duration;
};

} // namespace glpong
