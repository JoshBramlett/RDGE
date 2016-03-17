#include "menuscene.hpp"

#include <rdge/application.hpp>
#include <rdge/font.hpp>
#include <rdge/controls/button.hpp>
#include <rdge/gameobjects/text.hpp>

#include <functional>
#include <iostream>

namespace pong {

using namespace RDGE::GameObjects;
using namespace RDGE::Graphics;

namespace {
    constexpr RDGE::UInt32 BUTTON_WIDTH = 255;
    constexpr RDGE::UInt32 BUTTON_HEIGHT = 57;
}

MenuScene::MenuScene (const RDGE::Window& window)
    : RDGE::GameObjects::Scene(window)
{
    auto font_path = RDGE::Application::BasePath() + "res/fonts/OpenSansPX.ttf";
    auto font = std::make_shared<RDGE::Font>(font_path, 256);

    auto mid_width = (window.Width() / 2);
    auto mid_height = 50;
    auto location = Point(mid_width, mid_height);

    auto title = std::make_shared<Text>(
                                        "PONG",
                                        font,
                                        location,
                                        RDGE::Color::White(),
                                        RDGE::Font::RenderMode::Solid,
                                        TextAlignment::TopCenter
                                       );

    auto button_path = RDGE::Application::BasePath() + "res/images/menu.png";
    m_buttonsTexture = std::make_shared<RDGE::Texture>(window.Renderer(), button_path);

    // event handler function pointer
    auto fn_click = std::bind(&MenuScene::OnButtonClick, this, std::placeholders::_1);

    /*
     *  New Game Button
     */

    // base position on the window
    RDGE::Int32 x_position = (window.Width() / 2) - (BUTTON_WIDTH / 2);
    RDGE::Int32 y_position = (window.Height() / 2);

    // create clips - supporting 3 states (normal, pressed, and hover)
    // image has the following format:
    //   rows = { normal, pressed, hover }
    //   cols = { new game, settings, exit }
    RDGE::Graphics::Rect position { x_position, y_position, BUTTON_WIDTH, BUTTON_HEIGHT };
    RDGE::Graphics::Rect clip { 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
    RDGE::Graphics::Rect clip_pressed { 0, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
    RDGE::Graphics::Rect clip_hover { 0, BUTTON_HEIGHT*2, BUTTON_WIDTH, BUTTON_HEIGHT};
    auto clip_focus = RDGE::Graphics::Rect::Empty();
    auto clip_disabled = RDGE::Graphics::Rect::Empty();

    // create new game button
    auto btn_new_game = std::make_shared<RDGE::Controls::Button>(
                                                                 "btnNewGame",
                                                                 position,
                                                                 m_buttonsTexture,
                                                                 clip,
                                                                 clip_pressed,
                                                                 clip_focus,
                                                                 clip_hover,
                                                                 clip_disabled
                                                                );

    /*
     *  Settings Button
     */

    // amend position/clips for next button
    position.y += BUTTON_HEIGHT + 20;
    clip.x += BUTTON_WIDTH;
    clip_pressed.x += BUTTON_WIDTH;
    clip_hover.x += BUTTON_WIDTH;

    auto btn_settings = std::make_shared<RDGE::Controls::Button>(
                                                                 "btnSettings",
                                                                 position,
                                                                 m_buttonsTexture,
                                                                 clip,
                                                                 clip_pressed,
                                                                 clip_focus,
                                                                 clip_hover,
                                                                 clip_disabled
                                                                );

    /*
     *  Exit Button
     */

    position.y += BUTTON_HEIGHT + 20;
    clip.x += BUTTON_WIDTH;
    clip_pressed.x += BUTTON_WIDTH;
    clip_hover.x += BUTTON_WIDTH;

    auto btn_exit = std::make_shared<RDGE::Controls::Button>(
                                                             "btnExit",
                                                             position,
                                                             m_buttonsTexture,
                                                             clip,
                                                             clip_pressed,
                                                             clip_focus,
                                                             clip_hover,
                                                             clip_disabled
                                                            );

    // register event handlers
    btn_new_game->RegisterEventHandler(RDGE::Controls::ControlEventType::Click, fn_click);
    btn_settings->RegisterEventHandler(RDGE::Controls::ControlEventType::Click, fn_click);
    btn_exit->RegisterEventHandler(RDGE::Controls::ControlEventType::Click, fn_click);

    // Add entities to scene
    AddEntity("title", title);
    AddEntity("btnNewGame", btn_new_game);
    AddEntity("btnSettings", btn_settings);;
    AddEntity("btnExit", btn_exit);
}

void
MenuScene::OnButtonClick (const RDGE::Controls::ControlEventArgs& args)
{
    using namespace RDGE::GameObjects;

    std::cout << "Button " << args.id << " clicked" << std::endl;

    if (args.id == "btnNewGame")
    {
        TriggerEvent(
                     SceneEventType::RequestingPush,
                     { "new_game", SceneEventType::RequestingPush }
                    );
    }
    else if (args.id == "btnSettings")
    {
        TriggerEvent(
                     SceneEventType::RequestingPush,
                     { "settings", SceneEventType::RequestingPush }
                    );
    }
    if (args.id == "btnExit")
    {
        TriggerEvent(
                     SceneEventType::RequestingPop,
                     { "exit", SceneEventType::RequestingPop }
                    );
    }
}

} // namespace pong
