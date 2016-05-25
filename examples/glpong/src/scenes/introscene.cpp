#include "introscene.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/logger.hpp>


namespace glpong {

using namespace RDGE::Assets;
using namespace RDGE::GameObjects;
using namespace RDGE::Graphics;
using namespace RDGE::Math;

IntroScene::IntroScene (RDGE::GLWindow* window)
    : Scene::Scene()
    , m_layer(nullptr)
    , m_duration(SCENE_LENGTH)
{
    auto v = RDGE::Util::read_text_file("shaders/basic.vert");
    auto f = RDGE::Util::read_text_file("shaders/basic.frag");
    auto shader = std::make_unique<Shader>(v, f);

    auto aspect_ratio = window->TargetAspectRatio();
    auto ortho = mat4::orthographic(0.0f, aspect_ratio.w, 0.0f, aspect_ratio.h, -1.0f, 1.0f);

    m_layer = std::make_shared<Layer2D>(std::move(shader), ortho, 1.0f, 1);

    auto font = std::make_shared<Font>("fonts/OpenSansPX.ttf", 128);
    m_msg = std::make_shared<Text>("Hello, RDGE!", 1.0f, 4.0f, font, RDGE::Color::White());

    m_layer->AddRenderable(m_msg);
    AddLayer("logo", m_layer);




    //auto path = "res/fonts/OpenSansPX.ttf";
    //auto font = std::make_shared<RDGE::Font>(path, 96);

    //// Calculate base point where the text will be drawn
    //auto mid_width = (window->Width() / 2);
    //auto mid_height = (window->Height() / 2);
    //auto location = Point(mid_width, mid_height);

    //// Create and register text.  Alignment set to draw on center of the destination
    //auto hello = std::make_shared<Text>(
                                        //"Hello, RDGE!",
                                        //font,
                                        //location,
                                        //RDGE::Color::White(),
                                        //RDGE::Font::RenderMode::Solid,
                                        //TextAlignment::MiddleCenter
                                       //);

    //AddEntity("hello", hello);
}

void
IntroScene::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    Scene::ProcessUpdatePhase(ticks);

    m_duration -= ticks;

    // scene will live until duration has been met, then self terminate
    if (m_duration <= 0)
    {
        //TriggerEvent(
                     //SceneEventType::RequestingPop,
                     //{ "intro_end", SceneEventType::RequestingPop }
                    //);
        m_duration = 2000;
        return;
    }

    // fade out the welcome message
    double opacity = static_cast<double>(m_duration) / static_cast<double>(SCENE_LENGTH);
    m_msg->SetOpacity(static_cast<RDGE::UInt8>(opacity * 255));
}

} // namespace glpong
