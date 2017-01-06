#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/vops.hpp>
#include <rdge/system/window.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>

// *** Resource shoutouts ***
//
// Font provided by /u/teryror
//  - https://www.reddit.com/r/gamedev/comments/3clk56
// Images provided by kenney.nl
//  - http://kenney.nl/assets/shooting-gallery

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "03_spritesheet";
    settings.enable_png   = true;
    settings.enable_fonts = true;

    // 1) Initialize SDL
    Application app(settings);

    // TODO remove
    SetEventState(EventType::FingerDown, false);
    SetEventState(EventType::FingerUp, false);
    SetEventState(EventType::FingerMotion, false);
    SetEventState(EventType::MultiGesture, false);

    // 2) Create window
    Window window(settings);

    // 3) Load assets
    auto font = std::make_shared<Font>("res/OpenSansPX.ttf", 64);

    SpriteSheet stall_sheet("res/spritesheet_stall.json");
    SpriteLayer stall_layer;

    // TODO
    // 1) Curtain top sprites need to be created
    // 2) Move the stall_layer and sprite creation to a separate class
    // 3) Options for drawing the top curtain were to either string an array of sprites or
    //    bring that image outside the spritesheet, set the UV data to a larger multiple, and
    //    use OpenGL's texture wrapping.  Making changes in the shader to accommodate are not
    //    really a viable option.  Because the former was chosen calculating the scale wouldn't work
    //    because the vertices were set to sub-pixel, it had to be changed to a whole number.
    // 4) More thought needs to be put into the depth buffer
    // 5) Consider adding a helper class (or methods) for positioning that makes sense to the
    //    programmer.  e.g. Set the origin to a vertex and adding padding.
    // 6) The only reason my position numbers work is b/c I'm in high-res mode which has 2x the
    //    pixels of a traditional display, and the calculation for the ortho projection is the
    //    origin is at the center of the screen with the left and right edges set to negative
    //    and positive viewport.width/2.  Same for height.  I need a solution for getting these
    //    values.  Hoping implementing a camera will resolve it.

    float32 SCALE = 2.f;

    // 960 x 540
    //curtain.png
    //curtain_rope.png
    //curtain_straight.png
    //curtain_top.png

    // 4) Create renderable graphics
    const auto& curtain_part = stall_sheet["curtain.png"];
    auto size = static_cast<math::vec2>(curtain_part.size) * SCALE;

    auto left_curtain = std::make_shared<Sprite>(math::vec3(-970.f, (420.f - size.h), 0.009f),
                                                 size,
                                                 stall_sheet.texture,
                                                 curtain_part.coords);
    auto right_curtain = std::make_shared<Sprite>(math::vec3((970.f - size.w), (420.f - size.h), 0.009f),
                                                  size,
                                                  stall_sheet.texture,
                                                  curtain_part.coords);
    vops::FlipHorizontal(right_curtain->vertices);
    stall_layer.AddSprite(left_curtain);
    stall_layer.AddSprite(right_curtain);

    const auto& curtain_rope = stall_sheet["curtain_rope.png"];
    size = static_cast<math::vec2>(curtain_rope.size) * SCALE;
    auto left_rope = std::make_shared<Sprite>(math::vec3(-980.f, -35.f, 0.009f),
                                              size,
                                              stall_sheet.texture,
                                              curtain_rope.coords);
    auto right_rope = std::make_shared<Sprite>(math::vec3(980.f - size.w, -35.f, 0.009f),
                                               size,
                                               stall_sheet.texture,
                                               curtain_rope.coords);
    stall_layer.AddSprite(left_rope);
    stall_layer.AddSprite(right_rope);

    auto curtain_straight_part = stall_sheet["curtain_straight.png"];
    size = static_cast<math::vec2>(curtain_straight_part.size) * SCALE;

    int32 num = static_cast<int32>(1920.f / size.w) + 1;
    float32 x = -960.f;
    for (int32 i = 0; i < num; ++i)
    {
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, (540.f - size.h), 1.f),
                                                       size,
                                                       stall_sheet.texture,
                                                       curtain_straight_part.coords));

        x += size.w;
    }

    // 6) Create game loop
    bool running = true;
    Event event;
    while (running)
    {
        // 7) Poll for user input
        while (PollEvent(&event))
        {
            if (event.IsQuitEvent())
            {
                running = false;
                break;
            }
            else if (event.IsKeyboardEvent())
            {
                auto e = event.GetKeyboardEventArgs();
                if (e.Key() == KeyCode::Escape)
                {
                    running = false;
                    break;
                }
            }
        }

        // 8) Render to the screen using the layer
        window.Clear();

        stall_layer.Draw();

        window.Present();
    }

    return 0;
}
