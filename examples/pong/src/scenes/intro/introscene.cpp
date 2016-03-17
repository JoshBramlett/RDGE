#include "introscene.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/font.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/gameobjects/text.hpp>

#include <memory>

namespace pong {

using namespace RDGE::GameObjects;
using namespace RDGE::Graphics;

IntroScene::IntroScene (const RDGE::Window& window)
    : Scene::Scene(window)
    , m_duration(SCENE_LENGTH)
{
    auto path = RDGE::Application::BasePath() + "res/fonts/OpenSansPX.ttf";
    auto font = std::make_shared<RDGE::Font>(path, 96);

    // Calculate base point where the text will be drawn
    auto mid_width = (window.Width() / 2);
    auto mid_height = (window.Height() / 2);
    auto location = Point(mid_width, mid_height);

    // Create and register text.  Alignment set to draw on center of the destination
    auto hello = std::make_shared<Text>(
                                        "Hello, RDGE!",
                                        font,
                                        location,
                                        RDGE::Color::White(),
                                        RDGE::Font::RenderMode::Solid,
                                        TextAlignment::MiddleCenter
                                       );

    AddEntity("hello", hello);
}

void
IntroScene::Update (RDGE::UInt32 ticks)
{
    Scene::Update(ticks);

    auto hello = std::dynamic_pointer_cast<Text>(GetEntity("hello"));
    m_duration -= ticks;

    // scene will live until duration has been met, then self terminate
    if (m_duration <= 0)
    {
        TriggerEvent(
                     SceneEventType::RequestingPop,
                     { "intro_end", SceneEventType::RequestingPop }
                    );
        return;
    }

    // fade out the welcome message
    double opacity = static_cast<double>(m_duration) / static_cast<double>(SCENE_LENGTH);
    hello->SetOpacity(opacity);
}

} // namespace pong
