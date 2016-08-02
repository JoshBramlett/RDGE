#include "introscene.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/shaders/shader.hpp>
#include <rdge/math/vec3.hpp>
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
    // 1)  Create and compile our shader used by our rendering layer
    auto v = RDGE::Util::read_text_file("shaders/basic.vert");
    auto f = RDGE::Util::read_text_file("shaders/basic.frag");
    auto shader = std::make_unique<Shader>(v, f);

    // 2)  Set our projection matrix
    auto aspect_ratio = window->TargetAspectRatio();
    auto ortho = mat4::orthographic(0.0f, aspect_ratio.w, 0.0f, aspect_ratio.h, -1.0f, 1.0f);

    // 3)  Create our rendering layer
    m_layer = std::make_shared<Layer2D>(std::move(shader), ortho, 1.0f, 1);

    // 4)  Create our renderables (splash message)
    auto font = std::make_shared<Font>("fonts/OpenSansPX.ttf", 128);
    m_msg = std::make_shared<Text>("Hello, RDGE!", 0.0f, 0.0f, font, RDGE::Color::White());

    // 5)  Reset the position of the splash message to center on the screen
    auto msg_size = m_msg->Size();
    m_msg->SetPosition(vec3(
                            (aspect_ratio.w - msg_size.x) / 2.0f,
                            (aspect_ratio.h - msg_size.y) / 2.0f,
                            1.0f
                           ));

    // 6)  Add our renderables to the layer, and add the layer to the scene
    m_layer->AddRenderable(m_msg);
    AddLayer("logo", m_layer);
}

void
IntroScene::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    Scene::ProcessUpdatePhase(ticks);

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
    m_msg->SetOpacity(static_cast<float>(m_duration) / static_cast<float>(SCENE_LENGTH));
}

} // namespace glpong
