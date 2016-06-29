#include "guiscene.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/logger.hpp>

#include <functional>

#define BIND(x) std::bind(x, this, std::placeholders::_1)

namespace gui {

using namespace RDGE::Assets;
using namespace RDGE::GameObjects;
using namespace RDGE::Graphics;
using namespace RDGE::Math;
using namespace RDGE::Controls;

GUIScene::GUIScene (RDGE::GLWindow* window)
    : m_layer(nullptr)
    , m_button(nullptr)
{
    // 1)  Create and compile our shader used by our rendering layer
    auto v = RDGE::Util::read_text_file("shaders/basic.vert");
    auto f = RDGE::Util::read_text_file("shaders/basic.frag");
    auto shader = std::make_unique<Shader>(v, f);

    // 2)  Set our projection matrix
    auto aspect_ratio = window->TargetAspectRatio();
    auto ortho = mat4::orthographic(
                                    //-16.f, 16.f, -9.f, 9.f,
                                    //-aspect_ratio.w, aspect_ratio.w,
                                    //-aspect_ratio.h, aspect_ratio.h,
                                    0.0, aspect_ratio.w,
                                    0.0, aspect_ratio.h,
                                    -1.0f, 1.0f
                                   );

    // 3)  Create our rendering layer
    m_layer = std::make_shared<Layer2D>(std::move(shader), ortho, 1.0f, 10);

    auto button_config = RDGE::Util::read_text_file("textures/button.json");
    auto w = (256.f / window->Width()) * (aspect_ratio.w);
    auto h = (128.f / window->Height()) * (aspect_ratio.h);

    m_button = std::make_shared<Button>("btnTest", button_config, 1.5, 1.5, w, h);
    m_button->RegisterEventHandler(ControlEventType::Click, BIND(&GUIScene::OnButtonClick));
    m_button->RegisterEventHandler(ControlEventType::DoubleClick, BIND(&GUIScene::OnButtonDoubleClick));
    m_button->RegisterEventHandler(ControlEventType::MouseEnter, BIND(&GUIScene::OnMouseEnter));
    m_button->RegisterEventHandler(ControlEventType::MouseLeave, BIND(&GUIScene::OnMouseLeave));
    m_button->RegisterEventHandler(ControlEventType::MouseDown, BIND(&GUIScene::OnMouseDown));

    // 4)  Add our controls to the layer, and add the layer to the scene
    m_layer->AddGroup(m_button);
    AddLayer("gui", m_layer);
}

void
GUIScene::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    Scene::ProcessUpdatePhase(ticks);
}

void
GUIScene::OnButtonClick (const RDGE::Controls::ControlEventArgs& args)
{
    std::cout << "OnButtonClick button_id=" << args.id << std::endl;
}

void
GUIScene::OnButtonDoubleClick (const RDGE::Controls::ControlEventArgs& args)
{
    std::cout << "OnButtonDoubleClick button_id=" << args.id << std::endl;
}

void
GUIScene::OnMouseLeave (const RDGE::Controls::ControlEventArgs& args)
{
    std::cout << "OnMouseLeave button_id=" << args.id << std::endl;
}

void
GUIScene::OnMouseEnter (const RDGE::Controls::ControlEventArgs& args)
{
    std::cout << "OnMouseEnter button_id=" << args.id << std::endl;
}

void
GUIScene::OnMouseDown (const RDGE::Controls::ControlEventArgs& args)
{
    std::cout << "OnMouseDown button_id=" << args.id << std::endl;
}

void
GUIScene::OnFocusGained (const RDGE::Controls::ControlEventArgs& args)
{
    RDGE::Unused(args);
}

void
GUIScene::OnFocusLost (const RDGE::Controls::ControlEventArgs& args)
{
    RDGE::Unused(args);
}

} // namespace gui
